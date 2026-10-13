#ifndef ISORT_H
#define ISORT_H

template<class T>
class ISort {
public:
    virtual ~ISort() {}
    virtual void sort(T array[], int size, int (*comparator)(T&, T&) = 0) = 0;
};

template<class T>
class SortSimpleOrder {
public:
    static int compare4Ascending(T& lhs, T& rhs) {
        if (lhs < rhs) return -1;
        if (lhs > rhs) return +1;
        return 0;
    }

    static int compare4Desending(T& lhs, T& rhs) {
        if (lhs < rhs) return +1;
        if (lhs > rhs) return -1;
        return 0;
    }
};

#endif /* ISORT_H */
