#include "Common.h"
#include <Jolt/Physics/Collision/CollidePointResult.h>

void BindCollidePointResult(nb::module_ &m) {
    nb::class_<CollidePointResult> collidePointResultCls(m, "CollidePointResult",
        "Structure that holds the result of colliding a point against a shape");
    collidePointResultCls
        .def("get_early_out_fraction", &CollidePointResult::GetEarlyOutFraction,
            "Function required by the CollisionCollector. A smaller fraction is considered to be a 'better hit'. For point queries there is no sensible return value.")
        .def_rw("body_id", &CollidePointResult::mBodyID,
            "Body that was hit")
        .def_rw("sub_shape_id2", &CollidePointResult::mSubShapeID2,
            "Sub shape ID of shape that we collided against");
}