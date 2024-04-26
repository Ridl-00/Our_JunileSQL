#pragma once 
#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define MAX_BUFFER_SIZE 1024
typedef struct 
{
    char *buffer;
    size_t size;
    size_t input_length;

} Buffer;

Buffer *new_buffer(size_t size){

    if(size > 1024){
        puts("申请的缓存区过大");
        return NULL;
    }
    Buffer *buff = (Buffer *)malloc(sizeof(Buffer));
    if(buff != NULL){
        buff->buffer = (char *)malloc(sizeof(char) * size);
        buff->size = size;
        buff->input_length;
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
