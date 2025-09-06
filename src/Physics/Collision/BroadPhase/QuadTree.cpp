#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/QuadTree.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Collision/RayCast.h>

void BindQuadTree(nb::module_ &m) {
    nb::class_<QuadTree, NonCopyable> quadTreeCls(m, "QuadTree",
        "Internal tree structure in broadphase, is essentially a quad AABB tree.\n"
        "Tree is lockless (except for UpdatePrepare/Finalize() function), modifying objects in the tree will widen the aabbs of parent nodes to make the node fit.\n"
        "During the UpdatePrepare/Finalize() call the tree is rebuilt to achieve a tight fit again.");
    quadTreeCls
        .def("set_name", &QuadTree::SetName, "name"_a,
            "Name of the tree for debugging purposes")
        .def("get_name", &QuadTree::GetName, nb::rv_policy::reference)
        .def("has_bodies", &QuadTree::HasBodies,
            "Check if there is anything in the tree")
        .def("is_dirty", &QuadTree::IsDirty,
            "Check if the tree needs an UpdatePrepare/Finalize()")
        .def("can_be_updated", &QuadTree::CanBeUpdated,
            "Check if this tree can get an UpdatePrepare/Finalize() or if it needs a DiscardOldTree() first")
        .def("init", &QuadTree::Init, "allocator"_a,
            "Initialization")
        .def("discard_old_tree", &QuadTree::DiscardOldTree,
            "Will throw away the previous frame's nodes so that we can start building a new tree in the background")
        .def("get_bounds", &QuadTree::GetBounds,
            "Get the bounding box for this tree")
        .def("update_prepare", &QuadTree::UpdatePrepare, "bodies"_a, "tracking"_a, "update_state"_a, "full_rebuild"_a,
            "Update the broadphase, needs to be called regularly to achieve a tight fit of the tree when bodies have been modified.\n"
            "UpdatePrepare() will build the tree, UpdateFinalize() will lock the root of the tree shortly and swap the trees and afterwards clean up temporary data structures.")
        .def("update_finalize", &QuadTree::UpdateFinalize, "bodies"_a, "tracking"_a, "update_state"_a)
        .def("add_bodies_prepare", &QuadTree::AddBodiesPrepare, "bodies"_a, "tracking"_a, "body_i_ds"_a, "number"_a, "state"_a,
            "Prepare adding inNumber bodies at ioBodyIDs to the quad tree, returns the state in outState that should be used in AddBodiesFinalize.\n"
            "This can be done on a background thread without influencing the broadphase.\n"
            "ioBodyIDs may be shuffled around by this function.")
        .def("add_bodies_finalize", &QuadTree::AddBodiesFinalize, "tracking"_a, "number_bodies"_a, "state"_a,
            "Finalize adding bodies to the quadtree, supply the same number of bodies as in AddBodiesPrepare.")
        .def("add_bodies_abort", &QuadTree::AddBodiesAbort, "tracking"_a, "state"_a,
            "Abort adding bodies to the quadtree, supply the same bodies and state as in AddBodiesPrepare.\n"
            "This can be done on a background thread without influencing the broadphase.")
        .def("remove_bodies", &QuadTree::RemoveBodies, "bodies"_a, "tracking"_a, "body_i_ds"_a, "number"_a,
            "Remove inNumber bodies in ioBodyIDs from the quadtree.")
        .def("notify_bodies_aabb_changed", &QuadTree::NotifyBodiesAABBChanged, "bodies"_a, "tracking"_a, "body_i_ds"_a, "number"_a,
            "Call whenever the aabb of a body changes.")
        .def("cast_ray", &QuadTree::CastRay, "ray"_a, "collector"_a, "object_layer_filter"_a, "tracking"_a,
            "Cast a ray and get the intersecting bodies in ioCollector.")
        .def("collide_aa_box", &QuadTree::CollideAABox, "box"_a, "collector"_a, "object_layer_filter"_a, "tracking"_a,
            "Get bodies intersecting with inBox in ioCollector")
        .def("collide_sphere", &QuadTree::CollideSphere, "center"_a, "radius"_a, "collector"_a, "object_layer_filter"_a, "tracking"_a,
            "Get bodies intersecting with a sphere in ioCollector")
        .def("collide_point", &QuadTree::CollidePoint, "point"_a, "collector"_a, "object_layer_filter"_a, "tracking"_a,
            "Get bodies intersecting with a point and any hits to ioCollector")
        .def("collide_oriented_box", &QuadTree::CollideOrientedBox, "box"_a, "collector"_a, "object_layer_filter"_a, "tracking"_a,
            "Get bodies intersecting with an oriented box and any hits to ioCollector")
        .def("cast_aa_box", &QuadTree::CastAABox, "box"_a, "collector"_a, "object_layer_filter"_a, "tracking"_a,
            "Cast a box and get intersecting bodies in ioCollector")
        .def("find_colliding_pairs", &QuadTree::FindCollidingPairs, "bodies"_a, "active_bodies"_a, "num_active_bodies"_a, "speculative_contact_distance"_a, "pair_collector"_a, "object_layer_pair_filter"_a,
            "Find all colliding pairs between dynamic bodies, calls ioPairCollector for every pair found")
        // .def("get_ticks100_pct", &QuadTree::GetTicks100Pct,
        //     "Sum up all the ticks spent in the various layers")
        // .def("report_stats", &QuadTree::ReportStats, "ticks100_pct"_a,
        //     "Trace the stats of this tree to the TTY")
        ;

    nb::class_<QuadTree::Tracking> trackingCls(quadTreeCls, "Tracking",
        "Data to track location of a Body in the tree");
    trackingCls
        .def(nb::init<>(),
            "Constructor to satisfy the vector class")
        .def(nb::init<const QuadTree::Tracking &>(), "rhs"_a)
        .def_ro_static("C_INVALID_BODY_LOCATION", &QuadTree::Tracking::cInvalidBodyLocation,
            "Invalid body location identifier")
        // .def_rw("broad_phase_layer", &QuadTree::Tracking::mBroadPhaseLayer)
        // .def_rw("object_layer", &QuadTree::Tracking::mObjectLayer)
        // .def_rw("body_location", &QuadTree::Tracking::mBodyLocation)
        ;

    nb::class_<QuadTree::UpdateState> updateStateCls(quadTreeCls, "UpdateState");
    updateStateCls
        .def_rw("root_node_id", &QuadTree::UpdateState::mRootNodeID,
        "This will be the new root node id");

    nb::class_<QuadTree::AddState> addStateCls(quadTreeCls, "AddState",
        "Temporary data structure to pass information between AddBodiesPrepare and AddBodiesFinalize/Abort");
    addStateCls
        .def_rw("leaf_id", &QuadTree::AddState::mLeafID)
        .def_rw("leaf_bounds", &QuadTree::AddState::mLeafBounds);
}