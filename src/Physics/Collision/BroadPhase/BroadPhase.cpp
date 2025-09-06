#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Physics/Body/BodyManager.h>

void BindBroadPhase(nb::module_ &m) {
    nb::class_<BroadPhase, BroadPhaseQuery> broadPhaseCls(m, "BroadPhase",
        "Used to do coarse collision detection operations to quickly prune out bodies that will not collide.");
    broadPhaseCls
        .def("init", &BroadPhase::Init, "body_manager"_a, "layer_interface"_a,
            "Initialize the broadphase.\n"
            "Args:\n"
            "    body_manager (BodyManager*): The body manager singleton.\n"
            "    layer_interface (BroadPhaseLayerInterface): Interface that maps object layers to broadphase layers.\n"
            "Note that the broadphase takes a pointer to the data inside inObjectToBroadPhaseLayer so this object should remain static.")
        .def("optimize", &BroadPhase::Optimize,
            "Should be called after many objects have been inserted to make the broadphase more efficient, usually done on startup only")
        .def("frame_sync", &BroadPhase::FrameSync,
            "Must be called just before updating the broadphase when none of the body mutexes are locked")
        .def("lock_modifications", &BroadPhase::LockModifications,
            "Must be called before UpdatePrepare to prevent modifications from being made to the tree")
        .def("update_prepare", &BroadPhase::UpdatePrepare,
            "Update the broadphase, needs to be called frequently to update the internal state when bodies have been modified.\n"
            "The UpdatePrepare() function can run in a background thread without influencing the broadphase")
        .def("update_finalize", &BroadPhase::UpdateFinalize, "update_state"_a,
            "Finalizing the update will quickly apply the changes")
        .def("unlock_modifications", &BroadPhase::UnlockModifications,
            "Must be called after UpdateFinalize to allow modifications to the broadphase")
        .def("add_bodies_prepare", &BroadPhase::AddBodiesPrepare, "bodies"_a, "number"_a,
            "Prepare adding inNumber bodies at ioBodies to the broadphase, returns a handle that should be used in AddBodiesFinalize/Abort.\n"
            "This can be done on a background thread without influencing the broadphase.\n"
            "ioBodies may be shuffled around by this function and should be kept that way until AddBodiesFinalize/Abort is called.")
        .def("add_bodies_finalize", &BroadPhase::AddBodiesFinalize, "bodies"_a, "number"_a, "add_state"_a,
            "Finalize adding bodies to the broadphase, supply the return value of AddBodiesPrepare in inAddState.\n"
            "Please ensure that the ioBodies array passed to AddBodiesPrepare is unmodified and passed again to this function.")
        .def("add_bodies_abort", &BroadPhase::AddBodiesAbort, "bodies"_a, "number"_a, "add_state"_a,
            "Abort adding bodies to the broadphase, supply the return value of AddBodiesPrepare in inAddState.\n"
            "This can be done on a background thread without influencing the broadphase.\n"
            "Please ensure that the ioBodies array passed to AddBodiesPrepare is unmodified and passed again to this function.")
        .def("remove_bodies", &BroadPhase::RemoveBodies, "bodies"_a, "number"_a,
            "Remove inNumber bodies in ioBodies from the broadphase.\n"
            "ioBodies may be shuffled around by this function.")
        .def("notify_bodies_aabb_changed", &BroadPhase::NotifyBodiesAABBChanged, "bodies"_a, "number"_a, "take_lock"_a = true,
            "Call whenever the aabb of a body changes (can change order of ioBodies array)\n"
            "inTakeLock should be false if we're between LockModifications/UnlockModificiations in which case care needs to be taken to not call this between UpdatePrepare/UpdateFinalize")
        .def("notify_bodies_layer_changed", &BroadPhase::NotifyBodiesLayerChanged, "bodies"_a, "number"_a,
            "Call whenever the layer (and optionally the aabb as well) of a body changes (can change order of ioBodies array)")
        .def("find_colliding_pairs", &BroadPhase::FindCollidingPairs, "active_bodies"_a, "num_active_bodies"_a, "speculative_contact_distance"_a, "object_vs_broad_phase_layer_filter"_a, "object_layer_pair_filter"_a, "pair_collector"_a,
            "Find all colliding pairs between dynamic bodies\n"
            "Note that this function is very specifically tailored for the PhysicsSystem::Update function, hence it is not part of the BroadPhaseQuery interface.\n"
            "One of the assumptions it can make is that no locking is needed during the query as it will only be called during a very particular part of the update.\n"
            "Args:\n"
            "    active_bodies (BodyID*): is a list of bodies for which we need to find colliding pairs (this function can change the order of the ioActiveBodies array). This can be a subset of the set of active bodies in the system.\n"
            "    num_active_bodies (int): is the size of the ioActiveBodies array.\n"
            "    speculative_contact_distance (float): Distance at which speculative contact points will be created.\n"
            "    object_vs_broad_phase_layer_filter (ObjectVsBroadPhaseLayerFilter): is the filter that determines if an object can collide with a broadphase layer.\n"
            "    object_layer_pair_filter (ObjectLayerPairFilter): is the filter that determines if two objects can collide.\n"
            "    pair_collector (BodyPairCollector): receives callbacks for every body pair found.")
        .def("cast_aa_box_no_lock", &BroadPhase::CastAABoxNoLock, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a,
            "Same as BroadPhaseQuery::CastAABox but can be implemented in a way to take no broad phase locks.")
        .def("get_bounds", &BroadPhase::GetBounds,
            "Get the bounding box of all objects in the broadphase")
        // .def("report_stats", &BroadPhase::ReportStats,
        //     "Trace the collected broadphase stats in CSV form.\n"
        //     "This report can be used to judge and tweak the efficiency of the broadphase.")
            ;

    // TODO:
    nb::class_<BroadPhase::UpdateState> updateStateCls(broadPhaseCls, "UpdateState",
        "Context used during broadphase update");
    // updateStateCls
        // .def_rw("data", &BroadPhase::UpdateState::mData)
        ;
}