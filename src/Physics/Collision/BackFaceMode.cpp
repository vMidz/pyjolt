#include "Common.h"
#include <Jolt/Physics/Collision/BackFaceMode.h>

void BindBackFaceMode(nb::module_ &m) {
    nb::enum_<EBackFaceMode>(m, "EBackFaceMode",
        "How collision detection functions will treat back facing triangles")
        .value("IGNORE_BACK_FACES", EBackFaceMode::IgnoreBackFaces,
            "Ignore collision with back facing surfaces/triangles")
        .value("COLLIDE_WITH_BACK_FACES", EBackFaceMode::CollideWithBackFaces,
            "Collide with back facing surfaces/triangles");
}