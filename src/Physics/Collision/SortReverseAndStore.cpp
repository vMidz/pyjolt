#include "Common.h"
#include <Jolt/Physics/Collision/SortReverseAndStore.h>

void BindSortReverseAndStore(nb::module_ &m) {
    m.def("sort_reverse_and_store", [](Vec4Arg inValues, float inMaxValue, UVec4 &ioIdentifiers) {
        float outValues;
        auto r = SortReverseAndStore(inValues, inMaxValue, ioIdentifiers, &outValues);
        return nb::make_tuple(r, outValues);
    }, "values"_a, "max_value"_a, "identifiers"_a,
       "This function will sort values from high to low and only keep the ones that are less than inMaxValue.\n"
       "Args:\n"
       "    values (Vec4): Values to be sorted.\n"
       "    max_value (float): Values need to be less than this to keep them.\n"
       "    identifiers (UVec4): 4 identifiers that will be sorted in the same way as the values.\n"
       "    values (float*): The values are stored here from high to low.\n"
       "Returns:\n"
       "    int: The number of values that were kept.");

    m.def("count_and_sort_trues", &CountAndSortTrues, "value"_a, "identifiers"_a,
        "Shift the elements so that the identifiers that correspond with the trues in inValue come first.\n"
        "Args:\n"
        "    value (UVec4): Values to test for true or false.\n"
        "    identifiers (UVec4): the identifiers that are shifted, on return they are shifted.\n"
        "Returns:\n"
        "    int: The number of trues.");

}