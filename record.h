#ifndef RECORD_H
#define RECORD_H

#pragma once

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <ctime>
#include <vector>
#include <queue>

using namespace std;


// 定义记录结构体，包括整型属性A和字符串属性B
typedef struct record {
    int A;
    char B[20];

    int compareTo(record &r) { return A - r.A; };
} Record;


/**
 * 在内存中生成随机Records，最大值为max_value，返回指向Records的指针
 * @param num_records
 * @param a_max
 * @return
 */
Record *initialRecords(int num_records, int a_max) {
    srand(time(0)); // 使用当前时间作为种子
    Record *p_records = (Record *) malloc(num_records * sizeof(Record));
    if (p_records == NULL) {
        printf("Error: p_record is NULL\n");
        exit(1);
    }
    for (int i = 0; i < num_records; i++) {
        p_records[i].A = rand() % a_max;
        sprintf(p_records[i].B, "Record %d", p_records[i].A);
    }
    return p_records;
}

/**
 *  输出records的二进制文件数据
 * @param filename
 * @param print_num 需要输出的记录数,如果要读取整个文件中的所有记录，则可以设置 print_num 为文件中总记录数目
 */
void print_binFile_for_records(const char *filename, int print_num) {
    Record *p_records = (Record *) malloc(sizeof(Record) * print_num);
    if (p_records == NULL) {
        printf("Fail to malloc memory\n");
        return;
    }
    FILE *fr = fopen(filename, "rb");
    if (fr == NULL) {
        printf("Error opening file");
        exit(1);
    }
    size_t read_num = fread(p_records, sizeof(Record), print_num, fr);
    if (read_num < print_num) {
        printf("Error: read_num:%llu < print_num:%d\n", read_num, print_num);
    }
    for (int i = 0; i < read_num; i++) {
        printf("{record_id:%-6d A:%-6d  B:%-10s}\n", i + 1, p_records[i].A, p_records[i].B);
    }
    fclose(fr);
}


/**
 * 是一个用于动态创建二维数组的函数。它的输入参数是一个整数 m，表示需要创建的二维数组的行数，返回值是一个指向指针的指针，即一个二维数组。
 * 初始化一个二维记录数组,这个数组有m个指针, 每个指针分配n个空间
 * pRecords[m][n]
 * @param m 需要创建的二维数组的行数
 * @return pRecords_twoDim 二维数组
 */
Record **createRecordsTwoDim(int m) {
    Record **pRecords_twoDim = (Record **) malloc(sizeof(Record *) * m);
    if (pRecords_twoDim == NULL) {
        printf("no more memory");
        exit(1);
    }
    for (int i = 0; i < m; i++) {
        pRecords_twoDim[i] = NULL;
    }
    return pRecords_twoDim;
}

//败者树
#define LEAF_NUM 18 //叶子结点数目
#define A_MAX 30000
typedef int LoserTree[LEAF_NUM];     //败者树非终端结点(归并段下标)

/**
 * 用于将败者树中的叶子结点与其双亲结点中记录的败者进行比较，调整败者的值
 * @param tree
 * @param s
 * @param leaf
 */
void Adjust(LoserTree tree, int s, Record *leaf) {
    int temp = (s + LEAF_NUM) / 2;
    while (temp > 0) {
        if (leaf[s].A > leaf[tree[temp]].A) {
            int swap = s;
            s = tree[temp];
            tree[temp] = swap;
        }
        temp = temp / 2;
    }
    //最终将胜者的值赋给 tree[0]
    tree[0] = s;
}

/**
 * 用于初始化败者树
 * @param tree
 * @param leaf
 */
void InitialTree(LoserTree tree, Record *leaf) {
    leaf[LEAF_NUM].A = -1;
    for (int i = 0; i < LEAF_NUM; i++) {
        tree[i] = LEAF_NUM;
    }
    for (int i = LEAF_NUM - 1; i >= 0; i--) {
        Adjust(tree, i, leaf);
    }
}

/**
 * 求出一个数组中的最小值的索引(未优化版本）
 * @param pRecord
 * @param ways
 * @param a_max
 * @return 最小值的索引(如果已经读取完全部元组, 返回-1)
 */
int getMinNotOptimized(Record *pRecord, int ways, int a_max) {
    int min = -1;
    //
    for (int i = 0; i < ways; i++) {
        if (pRecord[i].A <= a_max) {
            min = i;
            break;
        }
    }
    // 已经读取完全部元组, 结束程序
    if (min == -1) {
        return min;
    }
    for (int i = 0; i < ways; i++) {
        if ((pRecord[i].A < pRecord[min].A) && pRecord[i].A <= a_max) {
            min = i;
        }
    }
    return min;
}

/**
 * 求出一个数组中的最小值的索引(败者树优化版本）
 * @param pRecord
 * @param ways
 * @param a_max
 * @return 最小值的索引(如果已经读取完全部元组, 返回-1)
 */
int getMinByLoserTree(LoserTree tree) {
    int min = tree[0];
    if (min == A_MAX + 1) {
        return -1;//已经读取完全部元组, 结束程序
    }
    return min;
}

/**
 * 求出一个数组中的最小值的索引(最小堆优化版本）
 * @param pRecord Record数组
 * @param ways
 * @param a_max
 * @return 最小值的索引(如果已经读取完全部元组, 返回-1)
 */
int getMinByHeap(priority_queue<int> min_heap) {
    int min = -1;
    //
    if (min_heap.empty()) {
        return min;
    }
    min = min_heap.top();
    min_heap.pop();
    return min;
}

#endif