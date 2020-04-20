/**
 * @file cbuffer.h
 * @brief Header file for Circular buffer implementation
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @author Laurens Miers <laurens.miers@mind.be>
 * @version v2.0
 * @date 2020-04-20
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include <stdio.h>

/* #define CBUFFER_DEBUG_OUTPUT */
#define CBUFFER_VALIDATE_PTRS
#define CBUFFER_VALIDATE_USAGE

#define CBUF_INFO(msg, ...) \
	printf("(INFO) %s: %s: (%d) " msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

#define CBUF_ERR(msg, ...) \
	printf("(ERROR) %s: %s: (%d) " msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

#ifdef CBUFFER_DEBUG_OUTPUT
#define CBUF_DEBUG(msg, ...) \
	printf("(DEBUG) " msg "\n", ## __VA_ARGS__)
#else
#define CBUF_DEBUG(msg, ...) while(0) {};
#endif

struct cbuffer_t {
        uint32_t nr_elements;
	uint32_t current_nr_elements;

        void **rp;
        void **wp;

#ifdef CBUFFER_VALIDATE_USAGE
	bool wp_in_use;
	bool rp_in_use;
#endif /* CBUFFER_VALIDATE_USAGE */

#ifdef CBUFFER_VALIDATE_PTRS
	uint8_t wp_index;
	uint8_t rp_index;
#endif /* CBUFFER_VALIDATE_PTRS */

        void **data;
};

struct cbuffer_t *cbuffer_init_cbuffer(int nr_elements);

static inline void *cbuffer_get_read_ptr(struct cbuffer_t *cbuf, int *error)
{
	if (!cbuf || !cbuf->rp) {
		CBUF_ERR("Invalid argument, cbuf || cbuf->rp == NULL");
		return NULL;
	}
	//TODO: Differentiate between NULLptr and empty
	if (!cbuf->current_nr_elements) {
		CBUF_DEBUG("Cbuffer is empty");
		return NULL;
	}

#ifdef CBUFFER_VALIDATE_USAGE
	if (cbuf->rp_in_use == true) {
		CBUF_ERR("RP Already taken!");
		return NULL;
	}
	cbuf->rp_in_use = true;
#endif /* CBUFFER_VALIDATE_USAGE */

	return *cbuf->rp;
}

static inline void *cbuffer_get_write_ptr(struct cbuffer_t *cbuf)
{
	if (!cbuf || !cbuf->wp) {
		CBUF_ERR("Invalid argument, cbuf || cbuf->wp == NULL");
		return NULL;
	}

#ifdef CBUFFER_VALIDATE_USAGE
	if (cbuf->wp_in_use == true) {
		CBUF_ERR("WP Already taken!");
		return NULL;
	}
	cbuf->wp_in_use = true;
#endif /* CBUFFER_VALIDATE_USAGE */

	return *cbuf->wp;
}

int cbuffer_return_read_ptr(struct cbuffer_t *cbuf);
int cbuffer_return_write_ptr(struct cbuffer_t *cbuf);

void cbuffer_destroy_cbuffer(struct cbuffer_t *cbuf);

#define CBUFFER_ALLOCATOR_HELPER(cbuf, type) \
	for (int i = 0; i < (cbuf)->nr_elements; i++) { \
		(cbuf)->data[i] = malloc(sizeof(type)); \
	}
