#include <stdio.h>
#include <stdlib.h>

#include "cbuffer.h"


int main(int argc, char **argv)
{
        int underrun = 0;
        struct cbuffer *cbuf = init_cbuffer(10);
        if (!cbuf) return -1;


        for(int i =0; i < 20; i++) {
                uint32_t *test = malloc(sizeof(uint32_t));
                *test = i;

                write_cbuffer(cbuf, (void*)test, &underrun);
                if (underrun) {
                        printf("Underrun detected element %d\n",i);
                }
                void *tmp = read_cbuffer(cbuf);
                printf("Read %d\n", *(uint32_t*)tmp);
                free(tmp);
        }
        void *tmp = read_cbuffer(cbuf);
        close_cbuffer(cbuf);
}
