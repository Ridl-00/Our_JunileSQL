#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COM_NUM 6

typedef enum{
    
    EXIT,
    CREATE,
    INSERT,
    DELETE,
    CHANGE,
    ANALYZE,
    INVALID_INPUT

}COMMANDS;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

const char *Commands[COM_NUM] = {".exit", ".create", ".insert", ".delete", ".change", ".analyze"};

void print_prompt() { printf("db > "); }

COMMANDS do_meta_command(char Buffer[]) {

    bool Valid_Input=0;
    for (int i=0;i<COM_NUM;i++){
        if(strcmp(Buffer, Commands[i])==0){
            Valid_Input=true;
            return (COMMANDS)i;
        }
    }
    if (!Valid_Input){
        // 如果不是识别的命令，则返回一个错误码
        return INVALID_INPUT;
    }
}


int main(int argc, char* argv[]) {

    char buffer[100];

    // FILE *InputFile, *OutputFile;
    // InputFile=fopen("example.csv", "r");
    // if (InputFile == NULL) {
    //     perror("Error opening file");
    //     return -1;
    // }
    

    while (true) {
        print_prompt();


        fgets(buffer, sizeof(buffer), stdin);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; // 移除换行符
        }

        // switch (do_meta_command(buffer)) {
        COMMANDS temp=do_meta_command(buffer);
        switch (temp){
            case (CREATE):
            case (INSERT):
            case (DELETE):
            case (CHANGE):
            case (ANALYZE):
                printf("Received command: %d\n", temp);
                continue;
            case (INVALID_INPUT):
                printf("Unrecognized command '%s'\n", buffer);
                continue;
            case (EXIT):
                printf("Exiting the program.\n");
                return 0;
        }
    }
}