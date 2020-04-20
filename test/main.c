#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

#include "cbuffer.h"
#include "tracer.h"

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

struct test_struct_t {
	int val;
};

int main(int argc, char **argv)
{
	struct cbuffer_t *cbuf = cbuffer_init_cbuffer(10);
	if (!cbuf) {
		return -1;
	}
	CBUFFER_ALLOCATOR_HELPER(cbuf, struct test_struct_t);

	double avg = 0.0;

#define NR_RUNS 10000
#define NR_INS 10000
	/* NONE Protected run */
	for (int runs = 0; runs < NR_RUNS; runs++) {
		struct time_trace_t *tracer = tracer_setup_time_trace();
		for (int i = 0; i < NR_INS; i++) {

			struct test_struct_t *tv = (struct test_struct_t *)cbuffer_get_write_ptr(cbuf);
			if (!tv) {
				CBUF_ERR("Something went wrong, ptr == NULL!\n");
				return -1;
			}
			tv->val = i;

			/* Simulate data corruption */
			/* struct test_struct_t tv_corruption; */
			/* cbuf->data = &tv_corruption; */

			int error = cbuffer_return_write_ptr(cbuf);

			tv = (struct test_struct_t *)cbuffer_get_read_ptr(cbuf, &error);
			if (!tv) {
				CBUF_ERR("Something went wrong, ptr == NULL!\n");
				continue;
			}
			error = cbuffer_return_read_ptr(cbuf);
		}
		tracer_time_trace_end(tracer);
		/* CBUF_INFO("%d insertions took: %.0lfms ", NR_INS, tracer->diff); */
		avg += tracer->diff;
		tracer_time_trace_free(tracer);
	}
	CBUF_INFO("%d insertions took: %.0lfms average over %d runs", NR_INS, avg / NR_RUNS, NR_RUNS);

	/* NONE Protected run */
	for (int runs = 0; runs < NR_RUNS; runs++) {
		struct time_trace_t *tracer = tracer_setup_time_trace();
		for (int i = 0; i < NR_INS; i++) {

			pthread_mutex_lock(&mtx);
			struct test_struct_t *tv = (struct test_struct_t *)cbuffer_get_write_ptr(cbuf);
			pthread_mutex_unlock(&mtx);
			if (!tv) {
				CBUF_ERR("Something went wrong, ptr == NULL!\n");
				return -1;
			}
			tv->val = i;

			/* Simulate data corruption */
			/* struct test_struct_t tv_corruption; */
			/* cbuf->data = &tv_corruption; */

			pthread_mutex_lock(&mtx);
			int error = cbuffer_return_write_ptr(cbuf);
			pthread_mutex_unlock(&mtx);

			pthread_mutex_lock(&mtx);
			tv = (struct test_struct_t *)cbuffer_get_read_ptr(cbuf, &error);
			pthread_mutex_unlock(&mtx);
			if (!tv) {
				CBUF_ERR("Something went wrong, ptr == NULL!\n");
				continue;
			}
			pthread_mutex_lock(&mtx);
			error = cbuffer_return_read_ptr(cbuf);
			pthread_mutex_unlock(&mtx);
		}
		tracer_time_trace_end(tracer);
		/* CBUF_INFO("%d insertions took: %.0lfms ", NR_INS, tracer->diff); */
		avg += tracer->diff;
		tracer_time_trace_free(tracer);
	}
	CBUF_INFO("%d insertions with mutex took: %.0lfms average over %d runs", NR_INS, avg / NR_RUNS, NR_RUNS);
	return 0;
}
