#include "Common.h"
#include <Jolt/Geometry/MortonCode.h>

void BindMortonCode(nb::module_ &m) {
    nb::class_<MortonCode>(m, "MortonCode")
        .def_static("expand_bits", &MortonCode::sExpandBits,
            "value"_a, "First converts a floating point value in the range [0, 1] to a 10 bit fixed point integer.\n"
            "Then expands a 10-bit integer into 30 bits by inserting 2 zeros after each bit.")

        .def_static("get_morton_code", &MortonCode::sGetMortonCode,
            "vector"_a, "vector_bounds"_a, "Calculate the morton code for inVector, given that all vectors lie in inVectorBounds");
}