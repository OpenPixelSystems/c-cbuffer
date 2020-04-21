/**
 * @file cbuffer.h
 * @brief Header file for Circular buffer implementation
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org> (v1.0, v2.1)
 * @author Laurens Miers <laurens.miers@mind.be> (v2.0)
 * @version v2.1
 * @date 2020-04-20
 */

/**
 * Notable changes with v2.0:
 * - Write / Read pointer validity no longer depends on current_nr_elements.
 *   While current_nr_elements is still susceptive to race-conditions it no longer
 *   impacts the read/write pointers
 * - Check pointer validity and wp/rp usages (check that no new pointer can be
 *   taken when the previous one has not been signaled as read or written)
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
        uint32_t nr_elements; //!< Number of elements available
	uint32_t current_nr_elements; //!< Current Number of elements

        void **rp; //!< Current read pointer
        void **wp; //!< Current write pointer

#ifdef CBUFFER_VALIDATE_USAGE
	bool wp_in_use; //!< Is a write pointer in use?
	bool rp_in_use; //!< Is a read pointer in use?
#endif /* CBUFFER_VALIDATE_USAGE */

#ifdef CBUFFER_VALIDATE_PTRS
	uint8_t wp_index; //!< Current wp index in data
	uint8_t rp_index; //!< Current rp index in data
#endif /* CBUFFER_VALIDATE_PTRS */

        void **data; //!< The actual data elements
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
