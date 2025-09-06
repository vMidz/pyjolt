#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseBruteForce.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Physics/Collision/RayCast.h>

void BindBroadPhaseBruteForce(nb::module_ &m)
{
    nb::class_<BroadPhaseBruteForce, BroadPhase> broadPhaseBruteForceCls(m, "BroadPhaseBruteForce",
        "Test BroadPhase implementation that does not do anything to speed up the operations. Can be used as a reference implementation.",
        nb::is_final());
    broadPhaseBruteForceCls
        .def("add_bodies_finalize", &BroadPhaseBruteForce::AddBodiesFinalize, "bodies"_a, "number"_a, "add_state"_a)
        .def("remove_bodies", &BroadPhaseBruteForce::RemoveBodies, "bodies"_a, "number"_a)
        .def("notify_bodies_aabb_changed", &BroadPhaseBruteForce::NotifyBodiesAABBChanged, "bodies"_a, "number"_a, "take_lock"_a)
        .def("notify_bodies_layer_changed", &BroadPhaseBruteForce::NotifyBodiesLayerChanged, "bodies"_a, "number"_a)
        .def("cast_ray", &BroadPhaseBruteForce::CastRay, "ray"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_aa_box", &BroadPhaseBruteForce::CollideAABox, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_sphere", &BroadPhaseBruteForce::CollideSphere, "center"_a, "radius"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_point", &BroadPhaseBruteForce::CollidePoint, "point"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_oriented_box", &BroadPhaseBruteForce::CollideOrientedBox, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("cast_aa_box_no_lock", &BroadPhaseBruteForce::CastAABoxNoLock, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("cast_aa_box", &BroadPhaseBruteForce::CastAABox, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("find_colliding_pairs", &BroadPhaseBruteForce::FindCollidingPairs, "active_bodies"_a, "num_active_bodies"_a, "speculative_contact_distance"_a, "object_vs_broad_phase_layer_filter"_a, "object_layer_pair_filter"_a, "pair_collector"_a)
        .def("get_bounds", &BroadPhaseBruteForce::GetBounds);
}