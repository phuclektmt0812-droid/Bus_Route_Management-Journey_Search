#include "QuickSort.h"
#include "tests/TestUtils.h"

struct Item {
    int key;
    string label;

    bool operator<(const Item& other) const { return key < other.key; }
    bool operator>(const Item& other) const { return key > other.key; }
};

static int byKeyDescending(Item& a, Item& b) {
    if (a.key > b.key) return -1;
    if (a.key < b.key) return 1;
    return 0;
}

static int pivotCalls = 0;
static int customPivot(int*, int size) {
    ++pivotCalls;
    return size > 0 ? 0 : 0;
}

int main(int argc, char* argv[]) {
    PublicTestSuite suite("Q4 - QuickSort");

    suite.add("Empty and single-element arrays", []() {
        int dummy = 123;
        QuickSort<int> sorter;
        sorter.sort(&dummy, 0);
        requireEqual(dummy, 123, "size 0 is unchanged");

        int single[] = {7};
        sorter.sort(single, 1);
        requireEqual(single[0], 7, "size 1 is unchanged");
    });

    suite.add("Default ascending order with duplicates", []() {
        int values[] = {5, 1, 4, 2, 3, 3};
        int expected[] = {1, 2, 3, 3, 4, 5};
        QuickSort<int> sorter;
        sorter.sort(values, 6);
        for (int i = 0; i < 6; ++i) {
            requireEqual(values[i], expected[i], "default ascending order");
        }
    });

    suite.add("Already sorted, reverse sorted, and all-equal input", []() {
        QuickSort<int> sorter;

        int alreadySorted[] = {1, 2, 3, 4, 5};
        sorter.sort(alreadySorted, 5);
        for (int i = 0; i < 5; ++i) requireEqual(alreadySorted[i], i + 1, "already sorted input");

        int reverseSorted[] = {5, 4, 3, 2, 1};
        sorter.sort(reverseSorted, 5);
        for (int i = 0; i < 5; ++i) requireEqual(reverseSorted[i], i + 1, "reverse sorted input");

        int allEqual[] = {4, 4, 4, 4, 4};
        sorter.sort(allEqual, 5);
        for (int value : allEqual) requireEqual(value, 4, "all-equal input");
    });

    suite.add("Custom comparator", []() {
        Item items[] = {{2, "b"}, {5, "e"}, {1, "a"}, {4, "d"}, {3, "c"}};
        QuickSort<Item> sorter;
        sorter.sort(items, 5, byKeyDescending);
        for (int i = 0; i < 5; ++i) {
            requireEqual(items[i].key, 5 - i, "negative comparator result means first item comes earlier");
        }
    });

    suite.add("Custom pivot selector", []() {
        int values[] = {8, 3, 7, 1, 6, 2, 5, 4};
        pivotCalls = 0;
        QuickSort<int> sorter(customPivot);
        sorter.sort(values, 8);
        requireTrue(pivotCalls > 0, "custom pivot selector is used");
        for (int i = 0; i < 8; ++i) {
            requireEqual(values[i], i + 1, "custom pivot sort result");
        }
    });

    return suite.run(argc, argv);
}
