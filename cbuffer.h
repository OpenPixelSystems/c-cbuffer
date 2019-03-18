/**
 * @file cbuffer.h
 * @brief Header file for Circular buffer implementation
 * @author Bram Vlerick
 * @version v1.0
 * @date 2019-03-18
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct cbuffer {
        uint32_t nr_elements;
        void **data;
        uint32_t read_pos;
        uint32_t write_pos;
};

struct cbuffer *init_cbuffer(int nr_elements);
int write_cbuffer(struct cbuffer *cbuf, void *data, int *underrun_detect);
void *read_cbuffer(struct cbuffer *cbuf);
void close_cbuffer(struct cbuffer *cbuf);
