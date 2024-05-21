#include <btree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//create a new node
Node *new_node(bool is_leaf) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;
    memset(node->keys, 0, sizeof(node->keys));
    if (is_leaf) {
        memset(node->records, 0, sizeof(node->records));
    } else {
        memset(node->children, 0, sizeof(node->children));
    }
    return node;
}
// 创建一个新的B+树
BPlusTree *new_bplus_tree(const char *filename, int order) {
    BPlusTree *tree = (BPlusTree *)malloc(sizeof(BPlusTree));
    tree->order = order;
    tree->root = new_node(true);
    return tree;
}

// 搜索操作：在节点中查找键的插入位置
int search_in_node(Node *node, int key) {
    int index = 0;
    while (index < node->num_keys && key > node->keys[index]) {
        index++;
    }
    return index;
}

// 在B+树中搜索关键字
Node *search(BPlusTree *tree, int key) {
    Node *current = tree->root;
    while (!current->is_leaf) {
        int index = search_in_node(current, key);
        current = (Node *)current->children[index];
    }
    return current;
}

// 插入操作：将关键字插入叶节点
void insert_into_leaf(Node *leaf, int key, StudentRecord record) {
    int index = search_in_node(leaf, key);
    // 移动后续的记录
    for (int i = leaf->num_keys; i > index; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->records[i] = leaf->records[i - 1];
    }
    // 插入新的键和记录
    leaf->keys[index] = key;
    leaf->records[index] = record;
    leaf->num_keys++;
}

// 将关键字插入B+树
void insert(BPlusTree *tree, int key, StudentRecord record) {
    Node *root = tree->root;
    if (root->num_keys == tree->order - 1) {
        // 根节点满时需要分裂根节点
        Node *new_root = new_node(false);
        new_root->children[0] = root;
        split_child(new_root, 0, root, tree);
        tree->root = new_root;
        // 将关键字插入到新根节点中
        insert_non_full(new_root, key, record, tree);
    } else {
        insert_non_full(root, key, record, tree);
    }
}

// 将关键字插入非满节点
void insert_non_full(Node *node, int key, StudentRecord record, BPlusTree *tree) {
    if (node->is_leaf) {
        // 如果是叶节点，直接插入关键字
        insert_into_leaf(node, key, record);
    } else {
        // 如果是内部节点，递归找到叶节点进行插入
        int index = search_in_node(node, key);
        Node *child = (Node *)node->children[index];
        if (child->num_keys == tree->order - 1) {
            // 子节点满时需要分裂
            split_child(node, index, child, tree);
            // 确定插入的子节点
            if (key > node->keys[index]) {
                child = (Node *)node->children[index + 1];
            }
        }
        insert_non_full(child, key, record, tree);
    }
}


void split_child(Node *parent, int index, Node *child, BPlusTree *tree) {
    // 创建一个新节点用于分裂
    Node *new_child_node = new_node(child->is_leaf);
    new_child_node->num_keys = tree->order / 2;

    // 复制子节点的后半部分到新节点
    for (int i = 0; i < new_child_node->num_keys; i++) {
        new_child_node->keys[i] = child->keys[i + tree->order / 2];
        if (child->is_leaf) {
            // 复制记录
            new_child_node->records[i] = child->records[i + tree->order / 2];
        } else {
            // 复制子节点
            new_child_node->children[i] = child->children[i + tree->order / 2];
        }
    }
    if (!child->is_leaf) {
        new_child_node->children[new_child_node->num_keys] = child->children[child->num_keys];
    }

    // 更新子节点的关键字数量
    child->num_keys = tree->order / 2;

    // 将新节点插入父节点
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = new_child_node;
    for (int i = parent->num_keys; i > index; i--) {
        parent->keys[i] = parent->keys[i - 1];
    }
    parent->keys[index] = child->keys[tree->order / 2];
    parent->num_keys++;

    // 如果子节点是叶节点，更新叶节点的链表
    if (child->is_leaf) {
        new_child_node->next = child->next;
        child->next = new_child_node;
    }
}


// 删除操作：从叶节点中删除关键字
void delete_from_leaf(Node *leaf, int key) {
    int index = search_in_node(leaf, key);
    if (index < leaf->num_keys && leaf->keys[index] == key) {
        // 删除关键字和记录
        for (int i = index; i < leaf->num_keys - 1; i++) {
            leaf->keys[i] = leaf->keys[i + 1];
            leaf->records[i] = leaf->records[i + 1];
        }
        leaf->num_keys--;
    }
}

// 从B+树中删除关键字
void delete(BPlusTree *tree, int key) {
    Node *root = tree->root;
    delete_from_node(root, key, tree);
    // 如果根节点的关键字数量为0
    if (root->num_keys == 0) {
        Node *new_root = NULL;
        if (root->is_leaf) {
            // 如果根节点是叶节点
            new_root = NULL;
        } else {
            // 如果根节点是内部节点
            new_root = root->children[0];
        }
        free(root);
        tree->root = new_root;
    }
}

// 从节点中删除关键字
void delete_from_node(Node *node, int key, BPlusTree *tree) {
    int index = search_in_node(node, key);
    if (index < node->num_keys && node->keys[index] == key) {
        if (node->is_leaf) {
            delete_from_leaf(node, key);
        } else {
            Node *left_child = (Node *)node->children[index];
            Node *right_child = (Node *)node->children[index + 1];
            if (left_child->num_keys > tree->order / 2) {
                Node *predecessor = left_child;
                while (!predecessor->is_leaf) {
                    predecessor = (Node *)predecessor->children[predecessor->num_keys];
                }
                int pred_key = predecessor->keys[predecessor->num_keys - 1];
                StudentRecord pred_record = predecessor->records[predecessor->num_keys - 1];
                node->keys[index] = pred_key;
                delete_from_node(left_child, pred_key, tree);
            } else if (right_child->num_keys > tree->order / 2) {
                Node *successor = right_child;
                while (!successor->is_leaf) {
                    successor = (Node *)successor->children[0];
                }
                int succ_key = successor->keys[0];
                StudentRecord succ_record = successor->records[0];
                node->keys[index] = succ_key;
                delete_from_node(right_child, succ_key, tree);
            } else {
                merge_children(node, index);
                delete_from_node(left_child, key, tree);
            }
        }
    } else {
        Node *child = (Node *)node->children[index];
        if (child->num_keys == tree->order / 2) {
            Node *left_sibling = index > 0 ? (Node *)node->children[index - 1] : NULL;
            Node *right_sibling = index < node->num_keys ? (Node *)node->children[index + 1] : NULL;
            if (left_sibling && left_sibling->num_keys > tree->order / 2) {
                borrow_from_left(node, index);
            } else if (right_sibling && right_sibling->num_keys > tree->order / 2) {
                borrow_from_right(node, index);
            } else if (left_sibling) {
                merge_children(node, index - 1);
                child = left_sibling;
            } else if (right_sibling) {
                merge_children(node, index);
            }
        }
        delete_from_node(child, key, tree);
    }
}
// 从左兄弟借用
void borrow_from_left(Node *parent, int index) {
    Node *child = (Node *)parent->children[index];
    Node *left_sibling = (Node *)parent->children[index - 1];

    // 将左兄弟的最后一个关键字和子节点借给子节点
    for (int i = child->num_keys; i > 0; i--) {
        child->keys[i] = child->keys[i - 1];
        if (!child->is_leaf) {
            child->children[i + 1] = child->children[i];
        }
    }
    if (!child->is_leaf) {
        child->children[0] = left_sibling->children[left_sibling->num_keys];
    }
    child->keys[0] = parent->keys[index - 1];
    parent->keys[index - 1] = left_sibling->keys[left_sibling->num_keys - 1];
    left_sibling->num_keys--;
    child->num_keys++;
}

// 从右兄弟借用
void borrow_from_right(Node *parent, int index) {
    Node *child = (Node *)parent->children[index];
    Node *right_sibling = (Node *)parent->children[index + 1];

    // 将右兄弟的第一个关键字和子节点借给子节点
    child->keys[child->num_keys] = parent->keys[index];
    if (!child->is_leaf) {
        child->children[child->num_keys + 1] = right_sibling->children[0];
    }
    parent->keys[index] = right_sibling->keys[0];
    for (int i = 0; i < right_sibling->num_keys - 1; i++) {
        right_sibling->keys[i] = right_sibling->keys[i + 1];
        if (!right_sibling->is_leaf) {
            right_sibling->children[i] = right_sibling->children[i + 1];
        }
    }
    right_sibling->num_keys--;
    child->num_keys++;
}

// 合并子节点
void merge_children(Node *parent, int index) {
    Node *left_child = (Node *)parent->children[index];
    Node *right_child = (Node *)parent->children[index + 1];

    // 将右子节点的内容合并到左子节点
    left_child->keys[left_child->num_keys] = parent->keys[index];
    for (int i = 0; i < right_child->num_keys; i++) {
        left_child->keys[left_child->num_keys + 1 + i] = right_child->keys[i];
        if (!left_child->is_leaf) {
            left_child->children[left_child->num_keys + 1 + i] = right_child->children[i];
        }
    }
    if (!left_child->is_leaf) {
        left_child->children[left_child->num_keys + right_child->num_keys + 1] = right_child->children[right_child->num_keys];
    }
    left_child->num_keys += right_child->num_keys + 1;

    // 移除父节点中的关键字和子节点
    for (int i = index; i < parent->num_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->num_keys--;

    // 释放右子节点的内存
    free(right_child);
}
