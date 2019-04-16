#include "mInt.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

void insertion_sort(mInt to_sort[], int size) {
    for (int i = 1; i < size; i++) {
        mInt tmp = to_sort[i];
        int j = i - 1;
        for (; j >= 0 && to_sort[j] > tmp; j--)
            to_sort[j + 1] = to_sort[j];
        to_sort[j + 1] = tmp;
    }
}

void bubble_sort(mInt to_sort[], int size) {
    for (int i = size; i >= 0; i--) {
        bool sorted = true;
        for (int j = 1; j < i; j++) {
            if (to_sort[j - 1] > to_sort[j]) {
                swap(to_sort[j - 1], to_sort[j]);
                sorted = false;
            }
        }
        if (sorted)
            break;
    }
}

void selection_sort(mInt to_sort[], int size) {
    for (int i = 0; i < size; i++) {
        mInt min_num = to_sort[i];
        int min_pos = i;
        for (int j = i + 1; j < size; j++)
            if (to_sort[j] < min_num) {
                min_num = to_sort[j];
                min_pos = j;
            }
        if (i != min_pos)
            swap(to_sort[i], to_sort[min_pos]);
    }
}

void shell_sort(mInt to_sort[], int size) {
    for (int sep = size / 2; sep; sep /= 2) {
        for (int i = sep; i < size; i++) {
            mInt tmp = to_sort[i];
            int j = i - sep;
            for (; j >= 0 && to_sort[j] > tmp; j -= sep)
                to_sort[j + sep] = to_sort[j];
            to_sort[j + sep] = tmp;
        }
    }
}

void quick_sort_iter(mInt to_sort[], int s, int e) {
    if (e - s < 1)
        return;
    mInt flag = to_sort[s];
    int i = s, j = e - 1;
    while (i < j) {
        while (i < j && to_sort[j] >= flag)
            j--;
        to_sort[i] = to_sort[j];
        while (i < j && to_sort[i] <= flag)
            i++;
        to_sort[j] = to_sort[i];
    }
    to_sort[i] = flag;
    quick_sort_iter(to_sort, s, i);
    quick_sort_iter(to_sort, i + 1, e);
}

inline void quick_sort(mInt to_sort[], int size) {
    quick_sort_iter(to_sort, 0, size);
}

void adjust_down(mInt to_sort[], int pos, int len) {
    int m;
    mInt tmp = to_sort[pos];
    while ((m = pos * 2 + 1) < len) {
        if (m + 1 < len && to_sort[m + 1] > to_sort[m])
            m++;
        if (to_sort[m] > tmp)
            to_sort[pos] = to_sort[m];
        else
            break;
        pos = m;
    }
    to_sort[pos] = tmp;
}

void heap_sort(mInt to_sort[], int size) {
    // build heap
    for (int i = size / 2; i >= 0; i--)
        adjust_down(to_sort, i, size);
    // heap sort
    int len = size - 1;
    while (len) {
        swap(to_sort[0], to_sort[len]);
        adjust_down(to_sort, 0, len);
        len--;
    }
}

void merge_sort_iter(mInt to_sort[], int s, int e, mInt *buffer) {
    if (e - s <= 1)
        return;
    int m = (s + e) / 2;
    merge_sort_iter(to_sort, s, m, buffer);
    merge_sort_iter(to_sort, m, e, buffer);

    // duplication
    int p1 = s, p2 = m;
    int i = 0;
    while (p1 < m && p2 < e) {
        if (to_sort[p1] < to_sort[p2])
            buffer[i++] = to_sort[p1++];
        else
            buffer[i++] = to_sort[p2++];
    }
    if (p1 < m)
        while (p1 < m)
            buffer[i++] = to_sort[p1++];
    else if (p2 < e)
        while (p2 < e)
            buffer[i++] = to_sort[p2++];
    // copy back
    for (int i = s; i < e; i++)
        to_sort[i] = buffer[i - s];
}

void merge_sort(mInt to_sort[], int size) {
    mInt *buffer = new mInt[size];
    merge_sort_iter(to_sort, 0, size, buffer);
    delete[] buffer;
}

void binary_insertion_sort(mInt to_sort[], int size) {
    for (int i = 1; i < size; i++) {
        // insertion position in [s,e]
        int s = 0, e = i, m;
        while (s < e) {
            m = (s + e) / 2;
            if (to_sort[m] <= to_sort[i])
                s = m + 1;
            else
                e = m;
        }
        // assert(s == e);
        mInt tmp = to_sort[i];
        for (int j = i; j > s; j--)
            to_sort[j] = to_sort[j - 1];
        to_sort[s] = tmp;
    }
}

template <class T> bool check_ascend(T arr[], int size) {
    for (int i = 1; i < size; i++)
        if (arr[i - 1] > arr[i])
            return false;
    return true;
}

template <class T>
void print_array(T arr, int size, int width = 10, char delim = '\t') {
    for (int i = 0; i < size; i++) {
        if (i && !(i % width))
            cout << endl;
        cout << arr[i] << delim;
    }
    cout << endl;
}

const int ITERATION_CNT = 100;
enum array_type { random, ascend, descend };
void (*funcs[])(mInt *, int) = {
    insertion_sort, bubble_sort, selection_sort, shell_sort,
    quick_sort,     heap_sort,   merge_sort,     binary_insertion_sort};
const char *names[] = {
    "insertion sort", "bubble sort", "selection sort", "shell sort",
    "quick sort",     "heap sort",   "merge sort",     "binary insertion sort"};
const int FUNC_CNT = sizeof(funcs) / sizeof(void (*)(mInt *, int));

int main(int argc, const char *argv[]) {
    int BUFFER_LENGTH = 200;
    array_type ARRAY_TYPE = random;
    if (argc > 1) {
        BUFFER_LENGTH = atoi(argv[1]);
        if (argc > 2) {
            switch (argv[2][0]) {
            case 'r':
                ARRAY_TYPE = random;
                break;
            case 'a':
                ARRAY_TYPE = ascend;
                break;
            case 'd':
                ARRAY_TYPE = descend;
                break;
            default:
                ARRAY_TYPE = random;
                cout << "Warning: Illegal input." << endl;
            }
        }
    }
    int *buffer = new int[BUFFER_LENGTH];
    static float stat[FUNC_CNT][ITERATION_CNT][2] = {0};
    srand(unsigned(time(NULL)));

    cout << "Sort algorithms comparison." << endl;
    cout << "N = " << BUFFER_LENGTH << " array type = ";
    switch (ARRAY_TYPE) {
    case random:
        cout << "random";
        break;
    case ascend:
        cout << "ascend";
        break;
    case descend:
        cout << "descend";
    }
    cout << endl << "iteration count = " << ITERATION_CNT << endl;
    for (int iter = 0; iter < ITERATION_CNT; iter++) {
        // cout << "iteration count #" << iter << endl;
        switch (ARRAY_TYPE) {
        case random:
            for (int i = 0; i < BUFFER_LENGTH; i++)
                buffer[i] = rand() & 0xffff;
            break;
        case ascend:
            for (int i = 0; i < BUFFER_LENGTH; i++)
                buffer[i] = i;
            break;
        case descend:
            for (int i = 0; i < BUFFER_LENGTH; i++)
                buffer[i] = BUFFER_LENGTH - i;
        }

        // print_array(buffer, BUFFER_LENGTH);
        for (int i = 0; i < FUNC_CNT; i++) {
            // cout << names[i] << ':' << endl;
            mInt *arr = int_array_to_mint_array(buffer, BUFFER_LENGTH);
            mInt::start_stat();
            funcs[i](arr, BUFFER_LENGTH);
            // print_array(arr, BUFFER_LENGTH);
            // cout << "\tmove time\t" << mInt::move_cnt << endl;
            // cout << "\tcomparison time\t" << mInt::cmp_cnt << endl;
            stat[i][iter][0] += mInt::move_cnt;
            stat[i][iter][1] += mInt::cmp_cnt;
            if (!check_ascend(arr, BUFFER_LENGTH)) {
                cout << "\tSorting error." << endl;
                break;
            }
            delete[] arr;
        }
    }

    // statistics
    cout << "Statistic values:" << endl;
    for (int i = 0; i < FUNC_CNT; i++) {
        cout << names[i] << ':' << endl;
        // calc mean value
        float mmt = 0, mct = 0, dmt = 0, dct = 0;
        for (int j = 0; j < ITERATION_CNT; j++) {
            mmt += stat[i][j][0];
            mct += stat[i][j][1];
        }
        mmt /= ITERATION_CNT;
        mct /= ITERATION_CNT;
        for (int j = 0; j < ITERATION_CNT; j++) {
            float k = stat[i][j][0] - mmt;
            dmt += k * k;
            k = stat[i][j][1] - mct;
            dct += k * k;
        }
        dmt /= ITERATION_CNT;
        dct /= ITERATION_CNT;
        dmt = sqrt(dmt);
        dct = sqrt(dct);
        cout << "\tmove time\t" << mmt << "\tsegma=" << dmt << endl;
        cout << "\tcomp time\t" << mct << "\tsegma=" << dct << endl;
    }
}