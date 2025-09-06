#include "Common.h"
#include <Jolt/Physics/Collision/CastResult.h>

void BindCastResult(nb::module_ &m) {
    nb::class_<BroadPhaseCastResult> broadPhaseCastResultCls(m, "BroadPhaseCastResult",
        "Structure that holds a ray cast or other object cast hit");
    broadPhaseCastResultCls
        .def("get_early_out_fraction", &BroadPhaseCastResult::GetEarlyOutFraction,
            "Function required by the CollisionCollector. A smaller fraction is considered to be a 'better hit'. For rays/cast shapes we can just use the collision fraction.")
        .def("reset", &BroadPhaseCastResult::Reset,
            "Reset this result so it can be reused for a new cast.")
        .def_rw("body_id", &BroadPhaseCastResult::mBodyID,
            "Body that was hit")
        .def_rw("fraction", &BroadPhaseCastResult::mFraction,
            "Hit fraction of the ray/object [0, 1], HitPoint = Start + mFraction * (End - Start)");

    nb::class_<RayCastResult, BroadPhaseCastResult> rayCastResultCls(m, "RayCastResult",
        "Specialization of cast result against a shape");
    rayCastResultCls
        .def_rw("sub_shape_id2", &RayCastResult::mSubShapeID2,
            "Sub shape ID of shape that we collided against");
}