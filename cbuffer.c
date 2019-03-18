#include "cbuffer.h"

struct cbuffer *init_cbuffer(int nr_elements)
{
        struct cbuffer *cbuf = malloc(sizeof(struct cbuffer));
        if (!cbuf) return NULL;
        memset(cbuf, 0, sizeof(struct cbuffer));
        
        cbuf->nr_elements = nr_elements;
        cbuf->lock = PTHREAD_MUTEX_INITIALIZER;

        cbuf->data = malloc(nr_elements * sizeof(void*));
        if (!cbuf->data) {
                free(cbuf);
                return NULL;
        }
        memset(cbuf->data, 0, nr_elements * sizeof(void*));

        cbuf->write_pos = 0;
        cbuf->read_pos = 0;

        return cbuf;
}

int write_cbuffer(struct cbuffer *cbuf, void *data, int *underrun_detect)
{
        if (cbuf->data[cbuf->write_pos]) {
                printf("Underrun detected, overwriting unread data\n");
                // Protect against data corruption when overwriting unread data
                pthread_mutex_lock(&cbuf->lock);
                // Clear unread data; Avoiding memleaks
                free(cbuf->data[cbuf->write_pos]);
                *underrun_detect = 1;
        }
        cbuf->data[cbuf->write_pos] = data;
        pthread_mutex_unlock(&cbuf->lock);
        cbuf->write_pos = (cbuf->write_pos + 1) % cbuf->nr_elements;
        return cbuf->write_pos;
}

void *read_cbuffer(struct cbuffer *cbuf)
{
        void *data;
        if (cbuf->read_pos == cbuf->write_pos) {
                printf("Buffer is empty\n");
                return NULL;
        }
        pthread_mutex_lock(&cbuf->lock);
        data = cbuf->data[cbuf->read_pos];
        cbuf->data[cbuf->read_pos] = NULL; //Clear to position to detect underrun
        cbuf->read_pos = (cbuf->read_pos + 1) % cbuf->nr_elements;
        pthread_mutex_unlock(&cbuf->lock);

        return data;
}

void close_cbuffer(struct cbuffer *cbuf)
{
       for (int i = 0; i < cbuf->nr_elements; i++) {
                free(cbuf->data[i]);
       } 
       free(cbuf->data);
       free(cbuf);
       cbuf = NULL;
}
