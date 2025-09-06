#include "Common.h"
#include <Jolt/Physics/Body/MotionType.h>

void BindMotionType(nb::module_ &m) {
    nb::enum_<EMotionType>(m, "EMotionType",
        "Motion type of a physics body")
        .value("STATIC", EMotionType::Static, "Non movable")
        .value("KINEMATIC", EMotionType::Kinematic, "Movable using velocities only, does not respond to forces")
        .value("DYNAMIC", EMotionType::Dynamic, "Responds to forces as a normal physics object");
}