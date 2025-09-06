#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuadTree.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Collision/RayCast.h>

void BindBroadPhaseQuadTree(nb::module_ &m) {
    nb::class_<BroadPhaseQuadTree, BroadPhase> broadPhaseQuadTreeCls(m, "BroadPhaseQuadTree",
        "Fast SIMD based quad tree BroadPhase that is multithreading aware and tries to do a minimal amount of locking.",
        nb::is_final());
    broadPhaseQuadTreeCls
        .def("init", &BroadPhaseQuadTree::Init, "body_manager"_a, "layer_interface"_a)
        .def("optimize", &BroadPhaseQuadTree::Optimize)
        .def("frame_sync", &BroadPhaseQuadTree::FrameSync)
        .def("lock_modifications", &BroadPhaseQuadTree::LockModifications)
        .def("update_prepare", &BroadPhaseQuadTree::UpdatePrepare)
        .def("update_finalize", &BroadPhaseQuadTree::UpdateFinalize, "update_state"_a)
        .def("unlock_modifications", &BroadPhaseQuadTree::UnlockModifications)
        .def("add_bodies_prepare", &BroadPhaseQuadTree::AddBodiesPrepare, "bodies"_a, "number"_a)
        .def("add_bodies_finalize", &BroadPhaseQuadTree::AddBodiesFinalize, "bodies"_a, "number"_a, "add_state"_a)
        .def("add_bodies_abort", &BroadPhaseQuadTree::AddBodiesAbort, "bodies"_a, "number"_a, "add_state"_a)
        .def("remove_bodies", &BroadPhaseQuadTree::RemoveBodies, "bodies"_a, "number"_a)
        .def("notify_bodies_aabb_changed", &BroadPhaseQuadTree::NotifyBodiesAABBChanged, "bodies"_a, "number"_a, "take_lock"_a)
        .def("notify_bodies_layer_changed", &BroadPhaseQuadTree::NotifyBodiesLayerChanged, "bodies"_a, "number"_a)
        .def("cast_ray", &BroadPhaseQuadTree::CastRay,
            "ray"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_aa_box", &BroadPhaseQuadTree::CollideAABox,
            "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_sphere", &BroadPhaseQuadTree::CollideSphere,
            "center"_a, "radius"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_point", &BroadPhaseQuadTree::CollidePoint,
            "point"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("collide_oriented_box", &BroadPhaseQuadTree::CollideOrientedBox,
            "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("cast_aa_box_no_lock", &BroadPhaseQuadTree::CastAABoxNoLock,
            "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("cast_aa_box", &BroadPhaseQuadTree::CastAABox,
            "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("find_colliding_pairs", &BroadPhaseQuadTree::FindCollidingPairs,
            "active_bodies"_a, "num_active_bodies"_a, "speculative_contact_distance"_a,
            "object_vs_broad_phase_layer_filter"_a, "object_layer_pair_filter"_a, "pair_collector"_a)
        .def("get_bounds", &BroadPhaseQuadTree::GetBounds)
        // .def("report_stats", &BroadPhaseQuadTree::ReportStats)
        ;
}