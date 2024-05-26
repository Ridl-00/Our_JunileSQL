#pragma once 
#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define MAX_FILENAME_LENGTH 11
#define BUFFER_SIZE 100
typedef struct 
{
    char *buffer;
    size_t size;

} Buffer;

Buffer *new_buffer(void);

void delete_buffer(Buffer *buff);

#endif