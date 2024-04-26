#include <buffer.h>
#include <stdlib.h>
#include <stddef.h>

Buffer *new_buffer(void){
    Buffer *buff = (Buffer *)malloc(sizeof(Buffer));
    buff->buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    buff->size = BUFFER_SIZE;
    return buff;
}
void delete_buffer(Buffer *buff){
    free(buff->buffer);
    free(buff);
}