#include "Common.h"

void BindSwizzle(nb::module_ &m) {
    m.attr("SWIZZLE_X") = (int)SWIZZLE_X;
    m.attr("SWIZZLE_Y") = (int)SWIZZLE_Y;
    m.attr("SWIZZLE_Z") = (int)SWIZZLE_Z;
    m.attr("SWIZZLE_UNUSED") = (int)SWIZZLE_UNUSED;
}