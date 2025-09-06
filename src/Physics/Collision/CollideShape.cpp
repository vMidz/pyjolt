#include "Common.h"
#include <Jolt/Physics/Collision/CollideShape.h>

void BindCollideShape(nb::module_ &m) {
    nb::class_<CollideShapeResult> collideShapeResultCls(m, "CollideShapeResult",
        "Class that contains all information of two colliding shapes");
    collideShapeResultCls
        .def(nb::init<>(),
            "Default constructor")
        .def(nb::init<Vec3Arg, Vec3Arg, Vec3Arg, float, const SubShapeID &, const SubShapeID &, const BodyID &>(), "contact_point_on1"_a, "contact_point_on2"_a, "penetration_axis"_a, "penetration_depth"_a, "sub_shape_id1"_a, "sub_shape_id2"_a, "body_id2"_a,
            "Constructor")
        .def("get_early_out_fraction", &CollideShapeResult::GetEarlyOutFraction,
            "Function required by the CollisionCollector. A smaller fraction is considered to be a 'better hit'. We use -penetration depth to get the hit with the biggest penetration depth")
        .def("reversed", &CollideShapeResult::Reversed,
            "Reverses the hit result, swapping contact point 1 with contact point 2 etc.")
        .def_rw("contact_point_on1", &CollideShapeResult::mContactPointOn1,
            "Contact point on the surface of shape 1 (in world space or relative to base offset)")
        .def_rw("contact_point_on2", &CollideShapeResult::mContactPointOn2,
            "Contact point on the surface of shape 2 (in world space or relative to base offset). If the penetration depth is 0, this will be the same as mContactPointOn1.")
        .def_rw("penetration_axis", &CollideShapeResult::mPenetrationAxis,
            "Direction to move shape 2 out of collision along the shortest path (magnitude is meaningless, in world space). You can use -mPenetrationAxis.Normalized() as contact normal.")
        .def_rw("penetration_depth", &CollideShapeResult::mPenetrationDepth,
            "Penetration depth (move shape 2 by this distance to resolve the collision)")
        .def_rw("sub_shape_id1", &CollideShapeResult::mSubShapeID1,
            "Sub shape ID that identifies the face on shape 1")
        .def_rw("sub_shape_id2", &CollideShapeResult::mSubShapeID2,
            "Sub shape ID that identifies the face on shape 2")
        .def_rw("body_id2", &CollideShapeResult::mBodyID2,
            "BodyID to which shape 2 belongs to")
        .def_rw("shape1_face", &CollideShapeResult::mShape1Face,
            "Colliding face on shape 1 (optional result, in world space or relative to base offset)")
        .def_rw("shape2_face", &CollideShapeResult::mShape2Face,
            "Colliding face on shape 2 (optional result, in world space or relative to base offset)");

    nb::class_<CollideSettingsBase> collideSettingsBaseCls(m, "CollideSettingsBase",
        "Settings to be passed with a collision query");
    collideSettingsBaseCls
        .def_rw("active_edge_mode", &CollideSettingsBase::mActiveEdgeMode,
            "How active edges (edges that a moving object should bump into) are handled")
        .def_rw("collect_faces_mode", &CollideSettingsBase::mCollectFacesMode,
            "If colliding faces should be collected or only the collision point")
        .def_rw("collision_tolerance", &CollideSettingsBase::mCollisionTolerance,
            "If objects are closer than this distance, they are considered to be colliding (used for GJK) (unit: meter)")
        .def_rw("penetration_tolerance", &CollideSettingsBase::mPenetrationTolerance,
            "A factor that determines the accuracy of the penetration depth calculation. If the change of the squared distance is less than tolerance * current_penetration_depth^2 the algorithm will terminate. (unit: dimensionless)")
        .def_rw("active_edge_movement_direction", &CollideSettingsBase::mActiveEdgeMovementDirection,
            "When mActiveEdgeMode is CollideOnlyWithActive a movement direction can be provided. When hitting an inactive edge, the system will select the triangle normal as penetration depth only if it impedes the movement less than with the calculated penetration depth.");

    nb::class_<CollideShapeSettings, CollideSettingsBase> collideShapeSettingsCls(m, "CollideShapeSettings",
        "Settings to be passed with a collision query");
    collideShapeSettingsCls
        .def_rw("max_separation_distance", &CollideShapeSettings::mMaxSeparationDistance,
            "When > 0 contacts in the vicinity of the query shape can be found. All nearest contacts that are not further away than this distance will be found (unit: meter)")
        .def_rw("back_face_mode", &CollideShapeSettings::mBackFaceMode,
            "How backfacing triangles should be treated");
}