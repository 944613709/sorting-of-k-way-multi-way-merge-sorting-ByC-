#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <time.h>
#include "externalSorting.h"

int main() {
    clock_t start = clock();
    generateRecords();
    int print_num_forExample = 5;//����չʾʾ���ĸ���
    std::cout << "������ɵ�����(��������ʾ��)��\n";
    print_binFile_for_records(path_input, print_num_forExample);
    std::cout << "\n";
    firstRun();
    secondRun();

    printf("�����ⲿ���������ʱ�䣺%fs\n\n", ((double) (clock() - start)) / CLK_TCK);
    std::cout << "�ⲿ������������������(��������ʾ��)��\n";
    print_binFile_for_records(path_output, print_num_forExample);
    return 0;
}

