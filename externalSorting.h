#pragma once
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include "record.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#define RECORDS_TOTAL_NUM 12345   // �����¼����
#define WAYS_NUM 18          // ����鲢�����·��
#define MAX_VALUE 30000     // �����¼��A���Ե����ֵ
#define BUFFER_TOTAL_SIZE 700      // �����ܻ�������С
#define SUB_BUF_SIZE 20      // �����Ӽ����ӻ�������С
#define OUTPUT_BUF_SIZE 200  // �����������С
const char* inputFile = "input_records.bin";  // �������ļ���
const char* outputFile = "ouput_records.bin"; // �������ļ���

FILE* subinput_fp[WAYS_NUM];  // �������ļ�ָ��
int recordNums[WAYS_NUM];     // ÿ���Ӽ��д洢�ļ�¼��


// �ڴ����������ݲ�д�����
/**
 * �������Records�����ֵΪmax_value
 * д��inputFile
 */
void generate_records() {
    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----��ʼ-����Records����\n\n", ((double)clock()) / CLK_TCK);

    assert(RECORDS_TOTAL_NUM <= BUFFER_TOTAL_SIZE * WAYS_NUM);
    assert(OUTPUT_BUF_SIZE + WAYS_NUM * (SUB_BUF_SIZE + 1) <= BUFFER_TOTAL_SIZE);
    FILE* fw;
    if ((fw = fopen(inputFile, "wb")) == NULL) {
        printf("Fail to open file\n");
        exit(0);
    }
    Record* pRecords = initial_records(RECORDS_TOTAL_NUM,MAX_VALUE);
    fwrite(pRecords, sizeof(Record), RECORDS_TOTAL_NUM, fw);
    fclose(fw);
    free(pRecords);

    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----���-����Records����\n\n", ((double)clock()) / CLK_TCK);
}



// �ȽϺ���������qsort�����жԼ�¼��������
int compare(const void* a, const void* b) {
    return ((Record*)a)->A - ((Record*)b)->A;
}


// һ������
void firstSort() {
    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----��ʼ-��һ������\n\n", ((double)clock()) / CLK_TCK);

    printf("�����Ӽ���С(%d): ", WAYS_NUM);
    for (int i = 0; i < WAYS_NUM; i++) {
        // ����ÿ���Ӽ��д洢�ļ�¼��
        recordNums[i] = i < RECORDS_TOTAL_NUM % WAYS_NUM ? RECORDS_TOTAL_NUM / WAYS_NUM + 1 : RECORDS_TOTAL_NUM / WAYS_NUM;
        printf("%d ", recordNums[i]);
    }
    printf("\n");

    // �������ж�ȡ����
    FILE* fr;
    if ((fr = fopen(inputFile, "rb")) == NULL) {
        printf("Fail to open file: input!\n");
        exit(0);
    }
    // ���������򲢱����ļ�
    for (int i = 0; i < WAYS_NUM; i++) {
        int n = recordNums[i];// ÿ���Ӽ��д洢�ļ�¼��
        Record* pRecord = (Record*)malloc(n * sizeof(Record));
        if (pRecord == NULL) {
            printf("Error: pRecord is NULL\n");
            exit(1);
        }
        // ��ȡ����
        fread(pRecord, sizeof(Record), n, fr);
        // ʹ�ÿ�������⺯����������
        qsort(pRecord,n , sizeof(Record), compare);

        // ����һ�˵��������Ӽ�����д���ļ�
        char subSet_name[20];
        sprintf(subSet_name, "subSet_%d.bin", i);
        subinput_fp[i] = fopen(subSet_name, "wb");
        if (subinput_fp[i] == NULL) {
            printf("Fail to open subSet%c!\n", i);
            exit(0);
        }
        fwrite(pRecord, sizeof(Record), n, subinput_fp[i]);
        free(pRecord);
        fclose(subinput_fp[i]);
        std::cout<<"��"<<i<<"���Ӽ��������"<<std::endl;
    }
    fclose(fr);
    //�����ǰϵͳʱ��
    printf("��ǰ����ʱ�䣺%fs----���-��һ������\n\n", ((double)clock()) / CLK_TCK);
}


// ��������
void secondSort() {
    printf("��ǰ����ʱ�䣺%fs----��ʼ-�ڶ�������\n\n", ((double)clock()) / CLK_TCK);
    // ���뻺����
    Record** pRecord2 = createRecordsTwoDim(WAYS_NUM);
    // ��ways���ļ�������
    // ��ȡ��һ���������Ӽ����ݣ�ÿ���Ӽ�����ȥ��ǰͷ��һ����Ԫ�أ��������Ӧ�����뻺����
    for (int i = 0; i < WAYS_NUM; i++) {
        char subSet_name[20];
        sprintf(subSet_name, "subSet_%d.bin", i);
        subinput_fp[i] = fopen(subSet_name, "rb");
        if (subinput_fp[i] == NULL) {
            printf("Fail to open subSet%c!\n", i);
            exit(0);
        }
        pRecord2[i] = (Record*)malloc(sizeof(Record) * SUB_BUF_SIZE);
        if (pRecord2[i] == NULL) {
            printf("Error: pRecord2[%d] is NULL\n",i);
            exit(1);
        }
        fread(pRecord2[i], sizeof(Record), SUB_BUF_SIZE, subinput_fp[i]);
    }

    // ��ʼ��������
    int buffer_waitNum[WAYS_NUM]; //
    int waitIndex[WAYS_NUM];        // ÿ���Ӽ�[��һ��]���ڵȵ�Ԫ���±�
    bool isEnd[WAYS_NUM];         // ÿ���Ӽ��Ƿ��Ѿ�����
    Record buffer_sorting[WAYS_NUM];     // ���򻺳���,(���������Ԫ��)
    Record buffer_output[OUTPUT_BUF_SIZE];  // ������ȴ��������Ԫ��(���������)
    int outputNum = 0;            // ��ǰ�����������Ԫ������
    // ��ʼ��
    for (int i = 0; i < WAYS_NUM; i++) {
        buffer_waitNum[i] = SUB_BUF_SIZE;
        isEnd[i] = false;   //�±�Ϊi���Ӽ��Ƿ��Ѿ�����
        buffer_sorting[i] = pRecord2[i][0]; // �ѵ�һ��Ԫ�����������
        waitIndex[i] = 1; // ��һ���ȴ���Ԫ���±�
    }
    int writeCount = 0; // �������������Ĵ���
    //��outputFile
    FILE* fw;
    if ((fw = fopen(outputFile, "wb")) == NULL) {
        printf("Fail to open file: buffer_output!\n");
        exit(0);
    }
    // ��·�鲢����
    while (true) {
        int min = getMin(buffer_sorting, WAYS_NUM, MAX_VALUE); // ��ǰ��СԪ�����ڵ��Ӽ�
        // ��������������
        if (min == -1) {
            printf("Output data to disk for %d times\n", writeCount);
            printf("All the records are sorted finally, End!\n");
            // �ͷ���Դ
            for (int i = 0; i < WAYS_NUM; i++) {
                fclose(subinput_fp[i]);
                free(pRecord2[i]);
            }
            fclose(fw);
            free(pRecord2);
            printf("��ǰ����ʱ�䣺%fs----����-�ڶ�������\n\n", ((double)clock()) / CLK_TCK);
            break;
        }
       
        buffer_output[outputNum] = buffer_sorting[min]; // �ѵ�ǰ���򻺳�������Сֵ�������������
        outputNum++;
        if (isEnd[min] == true) {
            buffer_sorting[min].A = MAX_VALUE + 1;// ���Ϊ�Ѿ����
        }
        else {
            buffer_sorting[min] = pRecord2[min][waitIndex[min]];  // ��min���ȴ���++
            waitIndex[min]++;
        }

        // ���ĳ���Ӽ��������Ѿ��Ƚ���
        if (waitIndex[min] == buffer_waitNum[min]) {
            size_t read_num=fread(pRecord2[min], sizeof(Record), SUB_BUF_SIZE, subinput_fp[min]);
            // �Ӽ��ѱȽ���(���һ��Ԫ����sorting[]�л�δ������)
            if (read_num == 0) {
                isEnd[min] = true;
                waitIndex[min] = -1;
            }
            else if (read_num < SUB_BUF_SIZE) {
                buffer_waitNum[min] = read_num;
                waitIndex[min] = 0; // ���ڵȴ���Ԫ������
            }
            else {
                waitIndex[min] = 0; // ���ڵȴ���Ԫ������
            } 
        }

        // �ж��Ƿ�������ݵ����
        if (outputNum == OUTPUT_BUF_SIZE) {
            // ����������������������������
            fwrite(buffer_output, sizeof(Record), OUTPUT_BUF_SIZE, fw);
            //printf("writeCount:%d   writeNums:%d  \n", writeCount+1, outputNum);
            outputNum = 0; // ����
            writeCount++;// �������������Ĵ���
        }
        else {
            // ���������δ������ȫ���Ӽ�������Ƚ���
            int flag = 0;
            // �ж��Ƿ������Ӽ����Ѿ��Ƚ���
            for (int i = 0; i < WAYS_NUM; i++) {
                if (buffer_sorting[i].A <= MAX_VALUE) {
                    flag = 1;
                    break;
                }
            }
            // ��������Ӽ����Ѿ��Ƚ��꣬�������������δ������Ҳ�ð����ʣ�������������
            if (flag == 0) {
                fwrite(buffer_output, sizeof(Record), outputNum, fw);
                //printf("writeCount:%d   writeNums:%d  \n", writeCount + 1, outputNum);
                outputNum = 0; // ����
                writeCount++;
            }
        }
    }
}
