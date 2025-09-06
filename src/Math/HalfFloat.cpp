#include "Common.h"
#include <Jolt/Math/HalfFloat.h>

void BindHalfFloat(nb::module_ &m) {
    m.def("from_float_fallback_nearest",
        &HalfFloatConversion::FromFloatFallback<HalfFloatConversion::ERoundingMode::ROUND_TO_NEAREST>,
        "value"_a,
        "Convert a float (32-bits) to a half float (16-bits), fallback version when no intrinsics available");

    m.def("to_float_fallback", &HalfFloatConversion::ToFloatFallback,
        "value"_a,
        "Convert 4 half floats (lower 64 bits) to floats, fallback version when no intrinsics available");

    m.def("from_float_nearest", &HalfFloatConversion::FromFloat<HalfFloatConversion::ERoundingMode::ROUND_TO_NEAREST>,
        "value"_a,
        "Convert a float (32-bits) to a half float (16-bits)");

    m.def("to_float", &HalfFloatConversion::ToFloat, "value"_a, "Convert 4 half floats (lower 64 bits) to floats");

    using namespace HalfFloatConversion;
    nb::enum_<ERoundingMode>(m, "ERoundingMode",
        "Define half-float rounding modes")
    .value("ROUND_TO_NEG_INF", ERoundingMode::ROUND_TO_NEG_INF,
        "Round to negative infinity")
    .value("ROUND_TO_POS_INF", ERoundingMode::ROUND_TO_POS_INF,
        "Round to positive infinity")
    .value("ROUND_TO_NEAREST", ERoundingMode::ROUND_TO_NEAREST,
        "Round to nearest value");

    m.attr("HALF_FLT_MAX") = HALF_FLT_MAX;
    m.attr("HALF_FLT_MAX_NEGATIVE") = HALF_FLT_MAX_NEGATIVE;
    m.attr("HALF_FLT_INF") = HALF_FLT_INF;
    m.attr("HALF_FLT_INF_NEGATIVE") = HALF_FLT_INF_NEGATIVE;
    m.attr("HALF_FLT_NANQ") = HALF_FLT_NANQ;
    m.attr("HALF_FLT_NANQ_NEGATIVE") = HALF_FLT_NANQ_NEGATIVE;
    m.attr("FLOAT_SIGN_POS") = FLOAT_SIGN_POS;
    m.attr("FLOAT_EXPONENT_POS") = FLOAT_EXPONENT_POS;
    m.attr("FLOAT_EXPONENT_BITS") = FLOAT_EXPONENT_BITS;
    m.attr("FLOAT_EXPONENT_MASK") = FLOAT_EXPONENT_MASK;
    m.attr("FLOAT_EXPONENT_BIAS") = FLOAT_EXPONENT_BIAS;
    m.attr("FLOAT_MANTISSA_BITS") = FLOAT_MANTISSA_BITS;
    m.attr("FLOAT_MANTISSA_MASK") = FLOAT_MANTISSA_MASK;
    m.attr("FLOAT_EXPONENT_AND_MANTISSA_MASK") = FLOAT_EXPONENT_AND_MANTISSA_MASK;
    m.attr("HALF_FLT_SIGN_POS") = HALF_FLT_SIGN_POS;
    m.attr("HALF_FLT_EXPONENT_POS") = HALF_FLT_EXPONENT_POS;
    m.attr("HALF_FLT_EXPONENT_BITS") = HALF_FLT_EXPONENT_BITS;
    m.attr("HALF_FLT_EXPONENT_MASK") = HALF_FLT_EXPONENT_MASK;
    m.attr("HALF_FLT_EXPONENT_BIAS") = HALF_FLT_EXPONENT_BIAS;
    m.attr("HALF_FLT_MANTISSA_BITS") = HALF_FLT_MANTISSA_BITS;
    m.attr("HALF_FLT_MANTISSA_MASK") = HALF_FLT_MANTISSA_MASK;
    m.attr("HALF_FLT_EXPONENT_AND_MANTISSA_MASK") = HALF_FLT_EXPONENT_AND_MANTISSA_MASK;
}