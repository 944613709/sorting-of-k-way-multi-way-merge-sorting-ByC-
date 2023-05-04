#pragma once
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>


// �����¼�ṹ�壬������������A���ַ�������B
typedef struct record_{
    int A;
    char B[20];
} Record;


// (�ڴ���)���������¼
Record* initial_records(int num_records,int max_value) {
    srand(time(0)); // ʹ�õ�ǰʱ����Ϊ����
    Record* pRecord = (Record*)malloc(num_records * sizeof(Record));
    if (pRecord == NULL) {
        printf("Error: pRecord is NULL\n");
        exit(1);
    }
    for (int i = 0; i < num_records; i++) {
        pRecord[i].A = rand() % max_value;
        sprintf(pRecord[i].B, "Record %d", pRecord[i].A);
    }
    return pRecord;
}

// ����������ļ�����
void print_records(const char* filename, int print_num) {
    Record* pRecords=(Record*)malloc(sizeof(Record)*print_num);
    if (pRecords == NULL) {
        printf("Fail to malloc memory\n");
        return;
    }
    FILE* fr = fopen(filename, "rb");
    if (fr == NULL) {
        printf("Error opening file");
        exit(1);
    }
    size_t read_num=fread(pRecords, sizeof(Record), print_num, fr);
    if (read_num < print_num) {
        printf("Error: read_num:%llu < print_num:%d\n", read_num, print_num);
    }
    for (int i = 0; i < read_num; i++) {
        printf("Seq:%-6d A:%-6d  B:%-10s\n", i + 1, pRecords[i].A, pRecords[i].B);
    }
    fclose(fr);
}


// ��ʼ��һ����ά��¼����
// ���������m��ָ��, ÿ��ָ�����n���ռ�
Record** createRecord2(int m) {
    Record** pRecord2 = (Record**)malloc(sizeof(Record*) * m);
    if (pRecord2 == NULL) {
        printf("no more memory");
        exit(1);
    }
    for (int i = 0; i < m; i++) {
        pRecord2[i] = NULL; // ��ÿ��������ÿ��ָ�����NULL
    }
    return pRecord2;
}


// ���һ�������е���Сֵ������
int getMin(Record* pRecord, int ways,int max_value) {
    int min = -1;
    for (int i = 0; i < ways; i++) {
        if (pRecord[i].A <= max_value) {
            min = i;
            break;
        }
    }
    // �Ѿ���ȡ��ȫ��Ԫ��, ��������
    if (min == -1) {
        return min;
    }
    for (int i = 0; i < ways; i++) {
        if ((pRecord[i].A < pRecord[min].A) && pRecord[i].A<=max_value) {
            min = i;
        }
    }
    return min;
}

