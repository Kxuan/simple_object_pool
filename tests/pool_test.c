
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <malloc.h>
#include "../pool.h"

struct object
{
	int index;
	struct object *next;
};

void functional_test()
{
	struct sop p;
	struct object *head = NULL, *tmp, *cur;
	int i = 0;
	sop_init(&p, sizeof(struct object), 78);

	while ((cur = sop_alloc(&p)) != NULL) {
		assert(cur->index == 0 && cur->next == NULL);
		cur->index = i++;
		if (!head) {
			head = cur;
		} else {
			tmp->next = cur;
		}
		tmp = cur;
		printf("index: %d, alloc_map: %016llx%016llx\n", cur->index, p.all_alu[1], p.all_alu[0]);
	}
	assert(i == 78);

	while (head) {
		tmp = head->next;
		sop_free(&p, head);
		head = tmp;
		i--;
		printf("index: %d, alloc_map: %016llx%016llx\n", i, p.all_alu[1], p.all_alu[0]);
	}
	assert(i == 0);

	while ((cur = sop_alloc(&p)) != NULL) {
		//assert(cur->next != NULL);
		printf("index: %d, alloc_map: %016llx%016llx\n", cur->index, p.all_alu[1], p.all_alu[0]);
		i++;
	}
	assert(i == 78);

	sop_cleanup(&p);
}

static double tsdiff(struct timespec *a, struct timespec *b)
{
	return (b->tv_sec - a->tv_sec) + ((b->tv_nsec - a->tv_nsec) / 1e9);
}

void performance_test()
{
	const int test_times = 10000000;
	int i;
	struct object *head, *tmp, *cur;
	struct timespec t_start, t_alloc, t_free;

	clock_gettime(CLOCK_MONOTONIC, &t_start);
	head = NULL;
	for (i = 0; i < test_times; i++) {
		cur = malloc(sizeof(struct object));
		assert(cur != NULL);
		if (!head) {
			head = cur;
		} else {
			tmp->next = cur;
		}
		tmp = cur;
	}
	clock_gettime(CLOCK_MONOTONIC, &t_alloc);
	while (head) {
		tmp = head->next;
		free(head);
		head = tmp;
	}
	clock_gettime(CLOCK_MONOTONIC, &t_free);

	printf("malloc  alloc: %lf s.  free: %lf s, total: %lf s.\n",
	       tsdiff(&t_start, &t_alloc),
	       tsdiff(&t_alloc, &t_free),
	       tsdiff(&t_start, &t_free));

	struct sop p;
	sop_init(&p, sizeof(struct object), test_times);

	clock_gettime(CLOCK_MONOTONIC, &t_start);
	head = NULL;
	for (i = 0; i < test_times; i++) {
		cur = sop_alloc(&p);
		assert(cur != NULL);
		if (!head) {
			head = cur;
		} else {
			tmp->next = cur;
		}
		tmp = cur;
	}
	clock_gettime(CLOCK_MONOTONIC, &t_alloc);
	while (head) {
		tmp = head->next;
		sop_free(&p, head);
		head = tmp;
	}
	clock_gettime(CLOCK_MONOTONIC, &t_free);
	sop_cleanup(&p);
	printf("sop     alloc: %lf s.  free: %lf s, total: %lf s.\n",
	       tsdiff(&t_start, &t_alloc),
	       tsdiff(&t_alloc, &t_free),
	       tsdiff(&t_start, &t_free));

}

int main()
{
	functional_test();
	for (int i = 0; i < 5; ++i) {
		performance_test();
	}
	return 0;
}