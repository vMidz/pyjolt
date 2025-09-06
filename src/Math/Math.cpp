#include "Common.h"
#include <Jolt/Math/Math.h>

void BindMath(nb::module_ &m) {
    m.attr("JPH_PI") = JPH_PI;

    m.def("degrees_to_radians", &DegreesToRadians, "value"_a,
        "Convert a value from degrees to radians");

    m.def("radians_to_degrees", &RadiansToDegrees, "value"_a,
        "Convert a value from radians to degrees");

    m.def("center_angle_around_zero", &CenterAngleAroundZero,
        "value"_a, "Convert angle in radians to the range \\f$[-\\pi, \\pi]\\f$");

    m.def("count_trailing_zeros", &CountTrailingZeros, "value"_a,
        "Compute number of trailing zero bits (how many low bits are zero)");

    m.def("count_leading_zeros", &CountLeadingZeros, "value"_a,
        "Compute the number of leading zero bits (how many high bits are zero)");

    m.def("count_bits", &CountBits, "value"_a,
        "Count the number of 1 bits in a value");

    m.def("get_next_power_of_2", &GetNextPowerOf2, "value"_a,
        "Get the next higher power of 2 of a value, or the value itself if the value is already a power of 2");
}