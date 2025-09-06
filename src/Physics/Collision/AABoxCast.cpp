#include "Common.h"
#include <Jolt/Physics/Collision/AABoxCast.h>

void BindAABoxCast(nb::module_ &m) {
    nb::class_<AABoxCast> aABoxCastCls(m, "AABoxCast",
        "Structure that holds AABox moving linearly through 3d space");
    aABoxCastCls
        .def_rw("box", &AABoxCast::mBox,
            "Axis aligned box at starting location")
        .def_rw("direction", &AABoxCast::mDirection,
            "Direction and length of the cast (anything beyond this length will not be reported as a hit)");
}