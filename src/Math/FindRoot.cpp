#include "Common.h"
#include "BindingUtility/Tuple.h"
#include <Jolt/Math/FindRoot.h>

template <typename T>
void BindInst(nb::module_ &m, const char *class_name) {
    m.def(class_name, [](const T inA, const T inB, const T inC) {
        T outX1, outX2;
        auto r = FindRoot<T>(inA, inB, inC, outX1, outX2);
        return CreateTuple(r, outX1, outX2);
    }, "a"_a, "b"_a, "c"_a,
       "Find the roots of \\f$inA \\: x^2 + inB \\: x + inC = 0\\f$.\n"
       "Returns:\n"
       "    tuple:\n"
       "    - int: The number of roots, actual roots in outX1 and outX2.\n"
       "If number of roots returned is 1 then outX1 == outX2.\n"
       "    - T: X1\n"
       "    - T: X2");
}

void BindFindRoot(nb::module_ &m) {
    BindInst<float>(m, "find_root_f");
    BindInst<int>(m, "find_root_i");
}