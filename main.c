#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

#include "cbuffer.h"


void *buffer_writer(void *data)
{
        int underrun = 0;
        struct cbuffer *cbuf = (struct cbuffer*)data;
        int i = 0;
        for(;;) {
                uint32_t *test = malloc(sizeof(uint32_t));
                *test = i;
                int pos = write_cbuffer(cbuf, (void*)test, &underrun);
                printf("Write: %d, %d\n",pos-1, i);
                if (underrun) {
                        printf("Underrun detected element %d\n", pos-1);
                        underrun = 0;
                }
                i++;
                usleep(5000);
        }
}

int main(int argc, char **argv)
{
        pthread_t writer;
        int underrun = 0;
        struct cbuffer *cbuf = init_cbuffer(100);
        if (!cbuf) return -1;

        pthread_create(&writer, NULL, buffer_writer, (void*)cbuf);
        sleep(1);

        for(;;) {
                void *tmp = NULL;
                int pos = read_cbuffer(cbuf, &tmp);
                if (tmp) { 
                        printf("Read %d, %d\n", pos-1, *(uint32_t*)tmp);
                        free(tmp);
                }
                usleep(5000);
        }
        close_cbuffer(cbuf);
}
