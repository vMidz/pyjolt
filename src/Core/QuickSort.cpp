#include "Common.h"
#include <Jolt/Core/QuickSort.h>

void BindQuickSort(nb::module_ &m) {
    m.def("quick_sort_int", [](Array<int> &array) {
        QuickSort(array.begin(), array.end());
    }, "Implementation of quick sort algorithm without comparator.");
}