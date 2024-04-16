#include <iostream>
#include <climits>
#include <omp.h>
#include <random>
using namespace std;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0,99);
const int rows = 5;
const int cols = 5;

void calculateSum(int arr[rows][cols], int &totalSum, int num_threads) {
    totalSum = 0;
#pragma omp parallel for reduction(+:totalSum) num_threads(num_threads)
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            totalSum += arr[i][j];
        }
    }
}
 void printArray(int arr[rows][cols]) {
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            cout << arr[i][j] << " ";
        }
        cout<<endl;
    }
}
void findMinRow(int arr[rows][cols], int &minRow, int &minRowSum, int num_threads) {
    minRow = 0;
    minRowSum = INT_MAX;
#pragma omp parallel num_threads(num_threads)
    {
        int localMinRow = 0;
        int localMinRowSum = INT_MAX;

#pragma omp for
        for(int i = 0; i < rows; ++i) {
            int rowSum = 0;
            for(int j = 0; j < cols; ++j) {
                rowSum += arr[i][j];
            }
            if(rowSum < localMinRowSum) {
                localMinRowSum = rowSum;
                localMinRow = i;
            }
        }

#pragma omp critical
        {
            if(localMinRowSum < minRowSum) {
                minRowSum = localMinRowSum;
                minRow = localMinRow;
            }
        }
    }
}

int main() {
    int arr[rows][cols];

    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            arr[i][j] = dis(gen);
        }
    }

    printArray(arr);
    int num_threads_main = 7;
    int num_threads_calc = 2;
    int num_threads_find = 5;

    omp_set_nested(1);

    omp_set_num_threads(num_threads_main);

    #pragma omp parallel sections //shared(totalSum, minRow, minRowSum)
    {
        #pragma omp section
        {
            int totalSum;
            double start = omp_get_wtime();
            calculateSum(arr, totalSum, num_threads_calc);
            double end = omp_get_wtime();
            cout << "Total sum: " << totalSum << endl;
            cout << "Time taken for total sum calculation: " << end - start << " seconds" << endl;
        }
        #pragma omp section
        {
            int minRow, minRowSum;
            double start = omp_get_wtime();
            findMinRow(arr, minRow, minRowSum, num_threads_find);
            double end = omp_get_wtime();
            cout << "Row with minimum sum: " << minRow << endl;
            cout << "Minimum row sum: " << minRowSum << endl;
            cout << "Time taken for finding minimum row sum: " << end - start << " seconds" << endl;
        }
    }
    return 0;
}
