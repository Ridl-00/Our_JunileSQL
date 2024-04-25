#pragma once 
#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define MAX_BUFFER_SIZE
typedef struct 
{
    char *buffer;
    size_t size;
    size_t input_length;

} Buffer;

Buffer *new_buffer(size_t size){

    Buffer *buff = (Buffer *)malloc(sizeof(Buffer));
    if(buff != NULL){
        buff->buffer = (char *)malloc(sizeof(char) * size);
        return buff;
    }
    else{
        puts("error!");
        return NULL;
    }

}

void delete_buffer(Buffer *buff){

    free(buff->buffer);
    free(buff);

}
#endif
