#include "Common.h"
#include <Jolt/Core/InsertionSort.h>
#include <Jolt/Core/Array.h>

template <typename T>
static void BindInst(nb::module_ &m, const char *name) {
    m.def(name, [](Array<T> array) {
        InsertionSort(array.begin(), array.end());
    }, "array"_a, "Implementation of insertion sort algorithm without comparator.");
}
void BindInsertionSort(nb::module_ &m) {
    BindInst<int>(m, "insertion_sort_int");
}