#include "Common.h"
#include <Jolt/Physics/Body/BodyType.h>

void BindBodyType(nb::module_ &m) {
    nb::enum_<EBodyType>(m, "EBodyType", "Type of body")
        .value("RIGID_BODY", EBodyType::RigidBody, "Rigid body consisting of a rigid shape")
        .value("SOFT_BODY", EBodyType::SoftBody, "Soft body consisting of a deformable shape");

    m.attr("BODY_TYPE_COUNT") = cBodyTypeCount;
}