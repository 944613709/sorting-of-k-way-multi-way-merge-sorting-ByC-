#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <time.h>
#include "externalSorting.h"

int main() {
    clock_t start = clock();
    generateRecords();
    int print_num_forExample = 5;//用来展示示例的个数
    std::cout << "随机生成的数据(部分数据示例)：\n";
    print_binFile_for_records(path_input, print_num_forExample);
    std::cout << "\n";
    firstRun();
    secondRun();

    printf("整个外部排序的运行时间：%fs\n\n", ((double) (clock() - start)) / CLK_TCK);
    std::cout << "外部排序结束后的最终数据(部分数据示例)：\n";
    print_binFile_for_records(path_output, print_num_forExample);
    return 0;
}

