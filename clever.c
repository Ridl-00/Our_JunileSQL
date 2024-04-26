#include <btree.h>
#include <print.h>
#include <openacc.h>
#include <operating.h>
#include <buffer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t MAX_FILENAME_LENGTH = 11;

typedef enum{
    
    EXIT,//退出数据库
    CREATE,//命令后携带一个filename参数，以filename为名创建一个数据库
    INSERT,
    DELETE,
    OPEN,
    SHOW,
    COM_NUM,
    INVALID_INPUT,

} COMMANDS;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

const char *Commands[COM_NUM] = {".exit", ".create", ".insert", ".delete", ".open", ".show"};

static char *f_fgets(char *buffer, int n, FILE *stream){
    char *ptr = fgets(buffer, n, stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0'; // 移除换行符
    }
    return ptr;
}

static void eat_line(void){
    while(getchar != '\n')
        continue;
}

static void print_prompt() { printf("db > "); }

COMMANDS do_meta_command(char Buffer[]) {

    bool Valid_Input=0;
    for(int i = 0; i < COM_NUM; i++){
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

    BPlusTree *current_tree = NULL;
    Buffer *input_buffer = new_buffer();
    char current_filename[MAX_FILENAME_LENGTH];
    // FILE *InputFile, *OutputFile;
    // InputFile=fopen("example.csv", "r");
    // if (InputFile == NULL) {
    //     perror("Error opening file");
    //     return -1;
    // }
    

    while (true) {
        print_prompt();


        f_fgets(input_buffer->buffer, input_buffer->size, stdin);

        // switch (do_meta_command(buffer)) {
        COMMANDS temp=do_meta_command(input_buffer->buffer);
        switch (temp){
            case (CREATE):
                puts("输入数据库名称:");
                f_fgets(input_buffer->buffer, MAX_FILENAME_LENGTH, stdin);
                strncpy(current_filename, input_buffer->buffer, MAX_FILENAME_LENGTH);
                current_tree = create_database(input_buffer->buffer, MAX_ORDER);
                continue;
            case (INSERT):
                if(current_tree == NULL){
                    puts("你需要先打开数据库");
                    break;
                }
                printf("请以 <Student_ID> <Age> <Name> <Class Number> <Political> 格式输入学生信息\n");
                printf("( <Political>参数 党员、预备党员、团员、群众、其他分别用: 0 , 1, 2, 3, 4 5 替代) \n");
                print_prompt();
                int temp_info = -1;
                fgets(input_buffer->buffer, input_buffer->size, stdin);
                StudentRecord *record = (StudentRecord *)malloc(sizeof(StudentRecord));
                sscanf(input_buffer->buffer, "%d %d %s %s %d",&record->student_id, &record->age,
                                                            record->name, record->class_number, 
                                                            &temp_info);
                record->political = (Political)temp_info;
                insert(current_tree, record->student_id, *record);
                continue;
            case (DELETE):
                puts("请输入被删除学生的学号");
                int key = -1; 
                while(scanf("%d", &key) != 1){
                    puts("错误的输入格式");
                    eat_line();
                }
                eat_line();
                delete_student_record(current_tree, key);
                continue;
            case (OPEN):
                puts("请输入要打开的数据库名称");
                f_fgets(input_buffer->buffer, MAX_FILENAME_LENGTH, stdin);
                strncpy(current_filename, input_buffer->buffer, MAX_FILENAME_LENGTH);
                current_tree = load_bplus_tree(input_buffer->buffer);
                continue;
            case (SHOW):
                print_bplus_tree(current_tree);
                printf("Received command: %d\n", temp);
                continue;
            case (INVALID_INPUT):
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
            case (EXIT):
                delete_buffer(input_buffer);
                printf("Exiting the program.\n");
                return 0;
        }
    }
}