#include <errno.h>
#include <memory.h>
#include <malloc.h>
#include <assert.h>
#include "pool.h"

#define ffs(x) __builtin_ffsll(x)
#define ALU_FULL ((sop_alu_t)-1)

static inline sop_alu_t last_alu_mask(int count)
{
	sop_alu_t alu = 0;
	if (count == 0) {
		return ALU_FULL;
	}
	for (int i = 0; i < count; ++i) {
		alu |= ((sop_alu_t) 1) << i;
	}
	return alu;
}

int sop_init(struct sop *pool, size_t object_size, size_t object_count)
{
	size_t map_unit_count, map_size, mem_size;
	uint8_t *mem;
	if (object_count == 0 || object_size == 0) {
		errno = EINVAL;
		return -1;
	}

	map_unit_count = (object_count - 1) / (8 * sizeof(sop_alu_t)) + 1;
	map_size = map_unit_count * sizeof(sop_alu_t);
	mem_size = map_size + object_count * object_size;
	mem = malloc(mem_size);
	if (!mem) {
		// ENOMEM
		return -1;
	}
	memset(mem, 0, mem_size);
	pool->all_alu = (sop_alu_t *) mem;
	pool->last_alu = pool->all_alu + (map_unit_count - 1);
	pool->next_alu = pool->all_alu;
	pool->last_alu_mask = last_alu_mask((int) (object_count % (8 * sizeof(sop_alu_t))));
	pool->objects = mem + map_size;
	pool->next_alu = pool->all_alu;
	pool->object_size = object_size;
	return 0;
}

void sop_cleanup(struct sop *pool)
{
	free(pool->all_alu);
}

volatile int zzzz = 1;

static inline sop_alu_t *next_alu(struct sop *pool)
{
	sop_alu_t *start, m;

	start = pool->next_alu;
	do {
		m = *pool->next_alu;
		if (pool->next_alu == pool->last_alu) {
			zzzz = 0;
		}
		if (pool->next_alu != pool->last_alu && m == ALU_FULL) {
			pool->next_alu++;
			continue;
		} else if (pool->next_alu == pool->last_alu && m == pool->last_alu_mask) {
			pool->next_alu = pool->all_alu;
			continue;
		}

		return pool->next_alu;
	} while (start != pool->next_alu);
	return NULL;
}

void *sop_alloc(struct sop *pool)
{
	size_t idx, full_idx;
	sop_alu_t *alu;

	alu = next_alu(pool);
	if (!alu) {
		errno = ENOMEM;
		return NULL;
	}

	idx = ffs(~*alu) - 1;
	full_idx = (alu - pool->all_alu) * (8 * sizeof(sop_alu_t)) + idx;
	*alu |= (sop_alu_t) 1 << idx;

	return pool->objects + pool->object_size * full_idx;
}

void sop_free(struct sop *pool, void *obj)
{
	ptrdiff_t pdiff;
	size_t full_idx;
	sop_alu_t *alu;

	pdiff = (uint8_t *) obj - pool->objects;
	assert(pdiff % pool->object_size == 0);

	full_idx = pdiff / pool->object_size;
	alu = pool->all_alu + full_idx / (8 * sizeof(sop_alu_t));
	*alu &= ~((sop_alu_t) 1 << (full_idx % (8 * sizeof(sop_alu_t))));
}
