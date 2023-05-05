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


// �����¼�ṹ�壬������������A���ַ�������B
typedef struct record {
    int A;
    char B[20];

    int compareTo(record &r) { return A - r.A; };
} Record;


/**
 * ���ڴ����������Records�����ֵΪmax_value������ָ��Records��ָ��
 * @param num_records
 * @param a_max
 * @return
 */
Record *initialRecords(int num_records, int a_max) {
    srand(time(0)); // ʹ�õ�ǰʱ����Ϊ����
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
 *  ���records�Ķ������ļ�����
 * @param filename
 * @param print_num ��Ҫ����ļ�¼��,���Ҫ��ȡ�����ļ��е����м�¼����������� print_num Ϊ�ļ����ܼ�¼��Ŀ
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
 * ��һ�����ڶ�̬������ά����ĺ������������������һ������ m����ʾ��Ҫ�����Ķ�ά���������������ֵ��һ��ָ��ָ���ָ�룬��һ����ά���顣
 * ��ʼ��һ����ά��¼����,���������m��ָ��, ÿ��ָ�����n���ռ�
 * pRecords[m][n]
 * @param m ��Ҫ�����Ķ�ά���������
 * @return pRecords_twoDim ��ά����
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

//������
#define LEAF_NUM 18 //Ҷ�ӽ����Ŀ
#define A_MAX 30000
typedef int LoserTree[LEAF_NUM];     //���������ն˽��(�鲢���±�)

/**
 * ���ڽ��������е�Ҷ�ӽ������˫�׽���м�¼�İ��߽��бȽϣ��������ߵ�ֵ
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
    //���ս�ʤ�ߵ�ֵ���� tree[0]
    tree[0] = s;
}

/**
 * ���ڳ�ʼ��������
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
 * ���һ�������е���Сֵ������(δ�Ż��汾��
 * @param pRecord
 * @param ways
 * @param a_max
 * @return ��Сֵ������(����Ѿ���ȡ��ȫ��Ԫ��, ����-1)
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
    // �Ѿ���ȡ��ȫ��Ԫ��, ��������
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
 * ���һ�������е���Сֵ������(�������Ż��汾��
 * @param pRecord
 * @param ways
 * @param a_max
 * @return ��Сֵ������(����Ѿ���ȡ��ȫ��Ԫ��, ����-1)
 */
int getMinByLoserTree(LoserTree tree) {
    int min = tree[0];
    if (min == A_MAX + 1) {
        return -1;//�Ѿ���ȡ��ȫ��Ԫ��, ��������
    }
    return min;
}

/**
 * ���һ�������е���Сֵ������(��С���Ż��汾��
 * @param pRecord Record����
 * @param ways
 * @param a_max
 * @return ��Сֵ������(����Ѿ���ȡ��ȫ��Ԫ��, ����-1)
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