#ifndef _SIMPLE_OBJECT_POOL_H_
#define _SIMPLE_OBJECT_POOL_H_

#include <sys/types.h>
#include <stdint.h>

typedef unsigned long long int sop_alu_t;
struct sop
{
	size_t object_size;
	sop_alu_t last_alu_mask;
	sop_alu_t *all_alu, *last_alu, *next_alu;
	uint8_t *objects;
};

int sop_init(struct sop *pool, size_t object_size, size_t object_count);

void sop_cleanup(struct sop *pool);

void *sop_alloc(struct sop *pool);

void sop_free(struct sop *pool, void *obj);

#endif //_SIMPLE_OBJECT_POOL_H_
