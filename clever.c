#include <btree.h>
#include <print.h>
#include <info_read.h>
#include <operating.h>
#include <buffer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mystrptime.h>

//#define NDEBUG
#include <assert.h>


bool save_flag = true;//记录数据改变是否被保存
BPlusTree *current_tree = NULL;//记录当前被载入内存中的B+树root地址
char current_filename[MAX_FILENAME_LENGTH];//记录当前打开的数据库文件名称

typedef enum{
    
    EXIT,//退出数据库
    CREATE,//命令后携带一个filename参数，以filename为名创建一个数据库
    INSERT,
    DELETE,
    OPEN,
    SHOW,// 展示完整数据库
    WRITE,
    SEARCH,// 按学号查询学生信息
    CHANGE,// 改变某一学生信息
    ANALYZE,// 按班级或年级统计学生政治面貌信息
    COM_NUM,
    INVALID_INPUT

} COMMANDS;
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

const char *Commands[COM_NUM] = {".exit", ".create", ".insert", ".delete", ".open", ".show", ".write", ".search", ".change", ".analyze"};

static void eat_line(void){
    while(getchar() != '\n')
        continue;
}

static char *f_fgets(char *buffer, int n, FILE *stream){
    char *ptr = fgets(buffer, n, stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0'; // 移除换行符
    }
    return ptr;
}

static void help_prompt(void) { puts("使用'.exit'命令可退出程序"); };
static void print_prompt(void) { printf("db/%s:> ", current_filename); }
static void save_prompt(const char *filename) { printf("数据已被保存至 %s文件\n", filename); }
static void wrong_format_prompt(void) { puts("错误的输入格式!"); };

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
    
    Buffer *input_buffer = new_buffer();
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
            {
                puts("输入数据库名称:");
                f_fgets(input_buffer->buffer, MAX_FILENAME_LENGTH, stdin);
                BPlusTree *temp = create_database(input_buffer->buffer, MAX_ORDER);
                if(current_tree == NULL && temp != NULL){
                    strncpy(current_filename, input_buffer->buffer, MAX_FILENAME_LENGTH);
                    current_tree = temp;
                }
                continue;
            }
            case (INSERT):
            {
                save_flag = false;
                if(current_tree == NULL){
                    puts("你需要先打开数据库");
                    break;
                }
                printf("请以 <Student_ID> <Age> <Name> <Class Number> <Political> 格式输入学生信息\n");
                printf("( <Political>参数可选： <党员>、<预备党员>、<团员>、<群众> 和 <其他> 分别用: 0 , 1, 2, 3, 4 5 替代) \n");
                print_prompt();
                int temp_info = -1;
                fgets(input_buffer->buffer, input_buffer->size, stdin);

                StudentRecord record;

                sscanf(input_buffer->buffer, "%d %d %s %s %d",&record.student_id, &record.age,
                                                            record.name, record.class_number, 
                                                            &temp_info);

                if(record.student_id < 0 || record.age < 0|| temp_info < 0){
                    wrong_format_prompt();
                    continue;
                }

                record.political = temp_info;
                read_feature_info(&record);
                insert_student_record(current_tree, record.student_id, record);
                continue;
            }
            case (DELETE):
            {
                save_flag = false;
                puts("请输入被删除学生的学号");
                int key = -1; 
                while(scanf("%d", &key) != 1){
                    wrong_format_prompt();
                    eat_line(); 
                }
                eat_line();
                delete_student_record(current_tree, key);
                continue;
            }
            case (OPEN):
            {
                puts("请输入要打开的数据库名称");
                f_fgets(input_buffer->buffer, MAX_FILENAME_LENGTH, stdin);
                BPlusTree *temp = load_bplus_tree(input_buffer->buffer);
                if(temp != NULL){
                    current_tree = temp;
                    strncpy(current_filename, input_buffer->buffer, MAX_FILENAME_LENGTH);
                }
                continue;
            }
            case (SHOW):
            {
                if(current_tree != NULL){
                    print_bplus_tree(current_tree);
                }
                continue;
            }
            case (WRITE):
            {
                if(current_tree == NULL){
                    puts("写入失败");
                    puts("当前未打开任意数据库。请先使用.open打开一个数据库");
                    continue;
                }
                else{
                    save_bplus_tree(current_tree, current_filename); 
                    save_prompt(current_filename);
                }

                save_flag = true;
                continue;
            }
            case (SEARCH):
            {
                puts("请输入要查询的学生学号");
                int key = -1; 
                while(scanf("%d", &key) != 1){
                    puts("当前查询的学生信息为");
                    eat_line();
                }
                eat_line();
                Node *cur=search(current_tree, key);
                int index=search_in_node(cur, key);
                puts("当前查询的学生信息为");
                print_record(&cur->records[index]);
                break;
            }
            case (CHANGE):
            {
                save_flag=false;
                puts("请输入要修改的学生的学号");
                int key = -1; 
                while(scanf("%d", &key) != 1){
                    wrong_format_prompt();
                    eat_line();
                }
                eat_line();

                Node *cur=search(current_tree, key);
                int index=search_in_node(cur, key);
                puts("当前要修改的学生信息为");
                print_record(&cur->records[index]);

                printf("<1: 姓名> <2: 班级> <3: 年龄> <4: 政治面貌> <5: 政治信息> (学号不可修改)\n");
                puts("请输入修改类型编号:");
                int order;
                while(scanf("%d", &order) != 1){
                    wrong_format_prompt();
                    eat_line();
                }
                switch (order){
                    case 1:
                    case 2:
                    case 3:
                        change_record_info(&cur->records[index], order);
                        break;
                    case 4:
                        change_record_politaical(&cur->records[index]);
                        break;
                    case 5:
                        change_record_poli_info(&cur->records[index]);
                    default:
                        wrong_format_prompt();
                        break;
                }
                puts("修改后的学生信息为");
                print_record(&cur->records[index]);
                eat_line();
                continue;
            }
            case (ANALYZE):
            {
                int op;
                
                if(current_tree != NULL){
                    puts("请输入查询信息");
                    puts("<grade> <class> 示例:2023 04932301; 23 all(查询整个年级)");
                    fgets(input_buffer->buffer, input_buffer->size, stdin);

                    char grade[10];
                    char class_number[10];
                    char all_sign[10];
                    sscanf("all", "%s", all_sign);
                    int ret=sscanf(input_buffer->buffer, "%s %s", grade, class_number);
                    if(ret!=2){
                        wrong_format_prompt();
                        continue;
                    }
                    if(strlen(grade)==4){
                        sscanf(grade, "%*2s%2s", grade);
                        // printf("年级：%s", grade);
                    }
                    if(strcmp(class_number, all_sign)==0){
                        analyze_grade(current_tree, grade);
                    }
                    else{
                        analyze_class(current_tree, class_number);
                    }
                }
                continue;
                
            }
            case (INVALID_INPUT):
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
            case (EXIT):
                delete_buffer(input_buffer);
                if(current_tree != NULL && !save_flag){
                    puts("数据已被修改，是否保存做出的修改(y/n)");
                    if(getchar() == 'y'){
                        save_bplus_tree(current_tree, current_filename);
                        save_prompt(current_filename);
                    }
                }
                free_bplus_tree(current_tree);
                printf("Exiting the program.\n");
                return 0;
        }
    }
}