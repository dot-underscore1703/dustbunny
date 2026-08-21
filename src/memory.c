#include <stdlib.h>
#include <stdio.h>

void chk_alloc(void* result) {
    if(result == NULL){
        perror("dustbunny: allocation failure!");
        exit(EXIT_FAILURE);
    }
}

void double_buffer_size(void **buf, size_t *bufsize) {
    *bufsize *= 2;

    *buf = realloc(*buf, *bufsize);
    chk_alloc(buf);

    //printf("Success!\n");
}