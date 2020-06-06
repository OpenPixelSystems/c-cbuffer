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

void *consumer_thread(void *data)
{
	struct cbuffer_t *cbuf = (struct cbuffer_t*)data;
	if (!cbuf) {
		CBUF_ERR("No cbuf found!");
		return NULL;
	}

	for (int i = 0; i < 1000000;) {
		struct test_struct_t *tv = (struct test_struct_t *)cbuffer_get_read_pointer(cbuf);
		if (!tv) {
			continue;
		}

		if (tv->val != i) {
			CBUF_ERR("Missing an element %d - %d", tv->val, i);
			exit(-1);
		}
		cbuffer_signal_element_read(cbuf);
		i++;
	}
}

int main(int argc, char **argv)
{
	struct cbuffer_t *cbuf = cbuffer_init_cbuffer(10);
	if (!cbuf) {
		return -1;
	}
	CBUFFER_ALLOCATOR_HELPER(cbuf, struct test_struct_t);

	double avg = 0.0;

#define NR_RUNS 1
#define NR_INS 1
	CBUF_INFO("Stage 1");
	/* NONE Protected run */
	for (int runs = 0; runs < NR_RUNS; runs++) {
		struct time_trace_t *tracer = tracer_setup_time_trace();
		for (int i = 0; i < NR_INS; i++) {

			struct test_struct_t *tv = (struct test_struct_t *)cbuffer_get_write_pointer(cbuf);
			if (!tv) {
				CBUF_ERR("Something went wrong, pointer == NULL!\n");
				return -1;
			}
			tv->val = i;

			/* Simulate data corruption */
			/* struct test_struct_t tv_corruption; */
			/* cbuf->data = &tv_corruption; */

			int error = cbuffer_signal_element_written(cbuf);
			if (error < 0) {
				return -1;
			}

			tv = (struct test_struct_t *)cbuffer_get_read_pointer(cbuf);
			if (!tv) {
				CBUF_ERR("Something went wrong, pointer == NULL!\n");
				continue;
			}
			error = cbuffer_signal_element_read(cbuf);
			if (error < 0) {
				return -1;
			}
		}
		tracer_time_trace_end(tracer);
		/* CBUF_INFO("%d insertions took: %.0lfms ", NR_INS, tracer->diff); */
		avg += tracer->diff;
		tracer_time_trace_free(tracer);
	}
	CBUF_INFO("%d insertions took: %.0lfms average over %d runs", NR_INS, avg / NR_RUNS, NR_RUNS);
	cbuffer_destroy_cbuffer(cbuf);

	CBUF_INFO("Stage 2");
	cbuf = cbuffer_init_cbuffer(3);

	pthread_t consumer;
	pthread_create(&consumer, NULL, consumer_thread, (void*)cbuf);

	CBUFFER_ALLOCATOR_HELPER(cbuf, struct test_struct_t);

	for (int i = 0; i < 1000000;) {
		struct test_struct_t *tv = (struct test_struct_t *)cbuffer_get_write_pointer(cbuf);
		if (!tv) {
			continue;
		}
		tv->val = i;
		cbuffer_signal_element_written(cbuf);
		i++;
	}
	pthread_join(consumer, NULL);

	return 0;
}
