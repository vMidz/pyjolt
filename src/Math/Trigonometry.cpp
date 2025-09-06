#include "Common.h"
#include <Jolt/Math/Trigonometry.h>

void BindTrigonometry(nb::module_ &m) {
    m.def("sin", &Sin, "x"_a, "Sine of x (input in radians)");
    m.def("cos", &Cos, "x"_a, "Cosine of x (input in radians)");
    m.def("tan", &Tan, "x"_a, "Tangent of x (input in radians)");
    m.def("a_sin", &ASin, "x"_a,
        "Arc sine of x (returns value in the range [-PI / 2, PI / 2])\n"
        "Note that all input values will be clamped to the range [-1, 1] and this function will not return NaNs like std::asin\n");
    m.def("a_cos", &ACos, "x"_a, "Arc cosine of x (returns value in the range [0, PI])\n"
                                 "Note that all input values will be clamped to the range [-1, 1] and this function will not return NaNs like std::acos");
    m.def("a_cos_approximate", &ACosApproximate, "x"_a, "An approximation of ACos, max error is 4.2e-3 over the entire range [-1, 1], is approximately 2.5x faster than ACos");
    m.def("a_tan", &ATan, "x"_a, "Arc tangent of x (returns value in the range [-PI / 2, PI / 2])");
    m.def("a_tan2", &ATan2, "y"_a, "x"_a,
        "Arc tangent of y / x using the signs of the arguments to determine the correct quadrant (returns value in the range [-PI, PI])");
}