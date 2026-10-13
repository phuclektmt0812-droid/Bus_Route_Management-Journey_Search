#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "ISort.h"
#include <stdexcept>
#include <utility>
using namespace std;

template<class T>
class QuickSort : public ISort<T> {
private:
    int (*pivotSelection)(T*, int);

public:
    QuickSort(int (*pivotSelection)(T*, int) = 0)
        : pivotSelection(pivotSelection) {}

    void sort(T array[], int size, int (*comparator)(T&, T&) = 0) override {
        // TODO Q4
        if (size <= 1) return;
        quickSort(array, 0, size - 1, comparator);
    }

private:
    void quickSort(T array[], int left, int right,int (*comparator)(T&, T&) = 0) {
        // TODO Q4
        if (left >= right) return;
        int pivotIndex = partition(array, left, right, comparator);
        if (left < pivotIndex) {
            quickSort(array, left, pivotIndex - 1, comparator);
        }
        if (pivotIndex + 1 < right) {
            quickSort(array, pivotIndex + 1, right, comparator);
        }
    }

    int partition(T array[], int left, int right,int (*comparator)(T&, T&) = 0) {
        // TODO Q4
        if (left > right) return left;
        int pivotPos = left + (right - left) / 2;
        if (pivotSelection != 0) {
            int relative = pivotSelection(array + left, right - left + 1);
            pivotPos = left + relative;
            if (pivotPos < left || pivotPos > right) {
                pivotPos = left + (right - left) / 2;
            }
        }
        T pivotValue = array[pivotPos];
        swap(array[pivotPos], array[right]);
        int storeIndex = left;
        for (int i = left; i < right; ++i) {
            int cmp;
            if (comparator != 0) {
                cmp = comparator(array[i], pivotValue);
            } else if (array[i] < pivotValue) {
                cmp = -1;
            } else if (array[i] > pivotValue) {
                cmp = 1;
            } else {
                cmp = 0;
            }
            if (cmp < 0) {
                swap(array[storeIndex], array[i]);
                ++storeIndex;
            }
        }
        swap(array[storeIndex], array[right]);
        return storeIndex;
    }
};

#endif
