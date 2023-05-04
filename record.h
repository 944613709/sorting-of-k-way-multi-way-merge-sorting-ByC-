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


/**
 * ���ڴ����������Records�����ֵΪmax_value������ָ��Records��ָ��
 * @param num_records
 * @param max_value
 * @return
 */
Record* initial_records(int num_records,int max_value) {
    srand(time(0)); // ʹ�õ�ǰʱ����Ϊ����
    Record* p_records = (Record*)malloc(num_records * sizeof(Record));
    if (p_records == NULL) {
        printf("Error: p_record is NULL\n");
        exit(1);
    }
    for (int i = 0; i < num_records; i++) {
        p_records[i].A = rand() % max_value;
        sprintf(p_records[i].B, "Record %d", p_records[i].A);
    }
    return p_records;
}

/**
 *  ���records�Ķ������ļ�����
 * @param filename
 * @param print_num ��Ҫ����ļ�¼��,���Ҫ��ȡ�����ļ��е����м�¼����������� print_num Ϊ�ļ����ܼ�¼��Ŀ
 */
void print_binFile_for_records(const char* filename, int print_num) {
    Record* p_records=(Record*)malloc(sizeof(Record)*print_num);
    if (p_records == NULL) {
        printf("Fail to malloc memory\n");
        return;
    }
    FILE* fr = fopen(filename, "rb");
    if (fr == NULL) {
        printf("Error opening file");
        exit(1);
    }
    size_t read_num=fread(p_records, sizeof(Record), print_num, fr);
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
Record** createRecordsTwoDim(int m) {
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



/**
 * ���һ�������е���Сֵ������
 * @param pRecord
 * @param ways
 * @param max_value
 * @return
 */
int getMin(Record* pRecord, int ways,int max_value) {
    int min = -1;
    //
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

