#include <btree.h>
#include <stdio.h>
#include <stdbool.h>

void serialize_node(Node *node, FILE *file) {
    fwrite(&node->is_leaf, sizeof(bool), 1, file);
    fwrite(&node->num_keys, sizeof(int), 1, file);
    fwrite(node->records, sizeof(StudentRecord), node->num_keys, file);
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            serialize_node(node->children[i], file);
        }
    }
}

Node *deserialize_node(FILE *file) {
    Node *node = (Node *)malloc(sizeof(Node));
    fread(&node->is_leaf, sizeof(bool), 1, file);
    fread(&node->num_keys, sizeof(int), 1, file);
    fread(node->records, sizeof(StudentRecord), node->num_keys, file);
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            node->children[i] = deserialize_node(file);
        }
    }
    return node;
}

void save_tree_to_file(BPlusTree *tree) {
    fseek(tree->file, 0, SEEK_SET);
    serialize_node(tree->root, tree->file);
    fflush(tree->file);
}

BPlusTree *create_database(const char *filename) {
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    tree->root = (Node *)malloc(sizeof(Node));
    tree->root->is_leaf = true;
    tree->root->num_keys = 0;
    tree->order = MAX_ORDER;
    tree->file = fopen(filename, "wb+");
    
    if (!tree->file) {
        perror("Failed to open file");
        free(tree);
        return NULL;
    }

    save_tree_to_file(tree);
    return tree;
}

BPlusTree *load_database(const char *filename) {
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    tree->order = MAX_ORDER;
    
    tree->file = fopen(filename, "rb+");
    if (!tree->file) {
        perror("Failed to open file");
        free(tree);
        return NULL;
    }
    
    tree->root = deserialize_node(tree->file);
    return tree;
}

// 在树中查找记录，返回找到的节点或 NULL
Node *search_in_db(BPlusTree *tree, int key) {
    Node *current = tree->root;
    while (current != NULL) {
        if (current->is_leaf) {
            for (int i = 0; i < current->num_keys; i++) {
                if (current->records[i].age == key) {
                    return current;
                }
            }
            return NULL;
        } else {
            int index = 0;
            while (index < current->num_keys && current->records[index].age < key) {
                index++;
            }
            current = current->children[index];
        }
    }
    return NULL;
}

// 在叶节点中插入键，处理键值重复的情况
void insert_into_leaf(Node *leaf, StudentRecord record) {
    int i = 0;
    while (i < leaf->num_keys && leaf->records[i].age < record.age) {
        i++;
    }
    
    // 检查键值是否重复
    if (i < leaf->num_keys && leaf->records[i].age == record.age) {
        // 键值重复，可以选择不插入或更新记录，根据需求定制
        leaf->records[i] = record; // 这里更新记录
        return;
    }
    
    // 向后移动记录以腾出插入空间
    for (int j = leaf->num_keys; j > i; j--) {
        leaf->records[j] = leaf->records[j - 1];
    }
    
    // 插入新记录
    leaf->records[i] = record;
    leaf->num_keys++;
}

// 在非满的节点中插入键，处理键值重复的情况
void insert_non_full(Node *node, StudentRecord record, BPlusTree *tree) {
    int index = 0;
    while (index < node->num_keys && node->records[index].age < record.age) {
        index++;
    }
    
    Node *child = node->children[index];
    
    // 如果子节点已满，进行分裂
    if (child->num_keys == MAX_ORDER - 1) {
        Node *new_child = (Node *)malloc(sizeof(Node));
        new_child->is_leaf = child->is_leaf;
        new_child->num_keys = MAX_ORDER / 2;
        
        // 将子节点分裂为两个节点
        for (int j = 0; j < MAX_ORDER / 2; j++) {
            new_child->records[j] = child->records[j + MAX_ORDER / 2];
        }
        
        if (!child->is_leaf) {
            for (int j = 0; j <= MAX_ORDER / 2; j++) {
                new_child->children[j] = child->children[j + MAX_ORDER / 2];
            }
        }
        
        child->num_keys = MAX_ORDER / 2;
        
        // 将新子节点插入到父节点的子节点列表中
        for (int j = node->num_keys; j > index; j--) {
            node->children[j + 1] = node->children[j];
        }
        node->children[index + 1] = new_child;
        
        // 将父节点的记录插入
        for (int j = node->num_keys - 1; j >= index; j--) {
            node->records[j + 1] = node->records[j];
        }
        node->records[index] = child->records[MAX_ORDER / 2 - 1];
        
        node->num_keys++;
        
        // 选择正确的子节点进行插入
        if (record.age > child->records[MAX_ORDER / 2 - 1].age) {
            child = new_child;
        }
    }
    
    // 插入记录
    if (child->is_leaf) {
        insert_into_leaf(child, record);
    } else {
        insert_non_full(child, record, tree);
    }
}

// 在树中插入记录，处理键值重复的情况
void insert_to_db(BPlusTree *tree, StudentRecord record) {
    Node *root = tree->root;
    
    if (root->num_keys == MAX_ORDER - 1) {
        // 根节点已满，分裂根节点
        Node *new_root = (Node *)malloc(sizeof(Node));
        new_root->is_leaf = false;
        new_root->num_keys = 1;
        new_root->records[0] = root->records[MAX_ORDER / 2 - 1];
        
        Node *new_child = (Node *)malloc(sizeof(Node));
        new_child->is_leaf = root->is_leaf;
        new_child->num_keys = MAX_ORDER / 2;
        
        // 将原根节点的后半部分复制到新子节点
        for (int j = 0; j < MAX_ORDER / 2; j++) {
            new_child->records[j] = root->records[j + MAX_ORDER / 2];
        }
        
        // 如果原根节点不是叶节点，复制后半部分的子节点
        if (!root->is_leaf) {
            for (int j = 0; j <= MAX_ORDER / 2; j++) {
                new_child->children[j] = root->children[j + MAX_ORDER / 2];
            }
        }
        
        // 更新原根节点的数量
        root->num_keys = MAX_ORDER / 2;
        
        // 更新新根节点的子节点
        new_root->children[0] = root;
        new_root->children[1] = new_child;
        
        tree->root = new_root;
        
        // 根据键值选择合适的子节点进行插入
        if (record.age > root->records[MAX_ORDER / 2 - 1].age) {
            insert_non_full(new_child, record, tree);
        } else {
            insert_non_full(root, record, tree);
        }
    } else {
        insert_non_full(root, record, tree);
    }
    
    // 保存树到文件
    save_tree_to_file(tree);
}