#include <btree.h>
#include <stdio.h>
#include <stdbool.h>
#include <operating.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ORDER 3

// 保存节点到文件
static void save_node(FILE *file, Node *node) {
    fwrite(node, sizeof(Node), 1, file);
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            save_node(file, (Node *)node->children[i]);
        }
    }
}

// 将B+树保存到文件
void save_bplus_tree(BPlusTree *tree, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("无法打开文件进行写入操作\n");
        return;
    }
    fwrite(&tree->order, sizeof(int), 1, file); // 写入树的顺序
    save_node(file, tree->root); // 保存根节点和所有子节点
    fclose(file);
}

// 从文件读取节点
Node *load_node(FILE *file, int order) {
    Node *node = (Node *)malloc(sizeof(Node));
    fread(node, sizeof(Node), 1, file);
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            node->children[i] = load_node(file, order);
        }
    }
    return node;
}

// 从文件加载B+树
BPlusTree *load_bplus_tree(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("无法打开文件进行读取操作\n");
        return NULL;
    }
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    fread(&tree->order, sizeof(int), 1, file); // 读取树的顺序
    tree->root = load_node(file, tree->order); // 加载根节点和所有子节点
    fclose(file);
    return tree;
}

// 插入学生记录
void insert_student_record(BPlusTree *tree, int key, StudentRecord record) {
    insert(tree, key, record);
}

// 读取学生记录
StudentRecord *read_student_record(BPlusTree *tree, int key) {
    Node *leaf = search(tree, key);
    if (leaf != NULL) {
        int index = search_in_node(leaf, key);
        if (index < leaf->num_keys && leaf->keys[index] == key) {
            return &leaf->records[index];
        }
    }
    return NULL;
}

// 删除学生记录
void delete_student_record(BPlusTree *tree, int key) {
    delete_(tree, key);
}

// 统计班级信息
void analyze_class(BPlusTree *tree, char *class_number)
{
    Class_info *count = (Class_info *)malloc(sizeof(Class_info));
    Node *root = tree->root;
    
    count_political_by_class(root, class_number, count);
    print_political_count_by_class(count);
}
// 统计个数
void count_political_by_class(Node * const root, const char *class_number, Class_info *count)
{
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
    // 初始化Class_info结构体
    strcpy(count->class_number, class_number);
    for (int i=0;i<5;i++){
        count->counts[i]=0;
    }

	int i;
	Node * c = root;
	while (!c->is_leaf)
		c = c->children[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
            if (strcmp(class_number, c->records[i].class_number)==0){
                count->counts[c->records[i].political]++;
            }
		}
		if (c->children[MAX_ORDER - 1] != NULL) {
			c = c->children[MAX_ORDER - 1];
		}
		else
			break;
	}
	printf("\n");
}
// 打印统计结果
void print_political_count_by_class(Class_info *count)
{
    printf("班级：%s\n", count->class_number);
    printf("中共党员：%d人 中共预备党员：%d人 共青团员：%d人 群众：%d人 其他：%d人\n", count->counts[0], count->counts[1], count->counts[2], count->counts[3], count->counts[4]);
}
// 修改记录基本信息
void change_record_info(StudentRecord *record, int order)
{
    if(order==1){
        puts("请输入新的姓名");
        char newname[50];
        scanf("%s", newname);
        strcpy(record->name, newname);
    }
    else if(order==2){
        puts("请输入新的班级");
        char newclass[10];
        scanf("%s", newclass);
        strcpy(record->name, newclass);
    }
    else if(order==3){
        puts("请输入新的年龄");
        scanf("%d", &record->age);
    }

    return;
}
// 修改记录政治面貌
void change_record_politaical(StudentRecord *record)
{
    puts("请输入新的政治面貌");
    printf("( <Political>参数可选： <党员>、<预备党员>、<团员>、<群众> 和 <其他> 分别用: 0 , 1, 2, 3, 4 5 替代) \n");
    int newpoli;
    scanf("%d", &newpoli);
    record->political=newpoli;
    read_feature_info(record);
    return;
}
// 修改记录政治信息
void change_record_poli_info(StudentRecord *record)
{
    puts("请输入新的政治面貌信息");
    read_feature_info(record);
    return;
}


extern BPlusTree *create_database(const char *filename, int order) {
    BPlusTree *tree = malloc(sizeof(BPlusTree));
    tree->root = new_node(true); // 创建根节点
    
    tree->file = fopen(filename, "wb");
    if (tree->file == NULL) {
        perror("Failed to open database file");
        free(tree);
        return NULL;
    }

    // 写入B+树的根节点和其他数据
    fwrite(&(tree->order), sizeof(int), 1, tree->file);
    fwrite(&(tree->root->is_leaf), sizeof(bool), 1, tree->file);
    fwrite(&(tree->root->num_keys), sizeof(int), 1, tree->file);
    fwrite(tree->root->keys, sizeof(int), MAX_ORDER - 1, tree->file);
    fwrite(tree->root->records, sizeof(StudentRecord), MAX_ORDER - 1, tree->file);
    fwrite(tree->root->children, sizeof(void *), MAX_ORDER, tree->file);

    printf("数据库'%s'已经创建并被打开\n", filename);
    // 关闭文件
    fclose(tree->file);

    return tree;
}