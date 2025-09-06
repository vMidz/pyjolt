#include "Common.h"
#include <Jolt/Physics/PhysicsUpdateContext.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Constraints/Constraint.h>

void BindPhysicsUpdateContext(nb::module_ &m) {

    nb::class_<PhysicsUpdateContext, NonCopyable> physicsUpdateContextCls(m, "PhysicsUpdateContext",
        "Information used during the Update call");
    physicsUpdateContextCls
        .def(nb::init<TempAllocator &>(), "temp_allocator"_a,
            "Destructor")
        .def("get_max_concurrency", &PhysicsUpdateContext::GetMaxConcurrency,
            "Maximum amount of concurrent jobs on this machine")
        .def_ro_static("C_MAX_CONCURRENCY", &PhysicsUpdateContext::cMaxConcurrency,
            "Maximum supported amount of concurrent jobs")
        // .def_rw("physics_system", &PhysicsUpdateContext::mPhysicsSystem,
        //     "The physics system we belong to")
        // .def_rw("temp_allocator", &PhysicsUpdateContext::mTempAllocator,
        //     "Temporary allocator used during the update")
        // .def_rw("job_system", &PhysicsUpdateContext::mJobSystem,
        //     "Job system that processes jobs")
        // .def_rw("barrier", &PhysicsUpdateContext::mBarrier,
        //     "Barrier used to wait for all physics jobs to complete")
        // .def_rw("step_delta_time", &PhysicsUpdateContext::mStepDeltaTime,
        //     "Delta time for a simulation step (collision step)")
        // .def_rw("warm_start_impulse_ratio", &PhysicsUpdateContext::mWarmStartImpulseRatio,
        //     "Ratio of this step delta time vs last step")
        // .def_rw("errors", &PhysicsUpdateContext::mErrors,
            // "Errors that occurred during the update, actual type is EPhysicsUpdateError")
        // .def_rw("active_constraints", &PhysicsUpdateContext::mActiveConstraints,
            // "Constraints that were active at the start of the physics update step (activating bodies can activate constraints and we need a consistent snapshot). Only these constraints will be resolved.")
        // .def_rw("body_pairs", &PhysicsUpdateContext::mBodyPairs,
            // "A list of body pairs found by the broadphase")
        // .def_rw("island_builder", &PhysicsUpdateContext::mIslandBuilder,
            // "Keeps track of connected bodies and builds islands for multithreaded velocity/position update")
        // .def_rw("steps", &PhysicsUpdateContext::mSteps)
        // .def_rw("num_soft_bodies", &PhysicsUpdateContext::mNumSoftBodies,
            // "Number of active soft bodies in the simulation")
        // .def_rw("soft_body_update_contexts", &PhysicsUpdateContext::mSoftBodyUpdateContexts,
            // "Contexts for updating soft bodies")
        // .def_rw("soft_body_to_collide", &PhysicsUpdateContext::mSoftBodyToCollide,
            // "Next soft body to take when running SoftBodyCollide jobs")
            ;

    nb::class_<PhysicsUpdateContext::BodyPairQueue> bodyPairQueueCls(physicsUpdateContextCls, "BodyPairQueue");
    bodyPairQueueCls
        // .def_rw("write_idx", &PhysicsUpdateContext::BodyPairQueue::mWriteIdx,
            // "Next index to write in mBodyPair array (need to add thread index * mMaxBodyPairsPerQueue and modulo mMaxBodyPairsPerQueue)")
        // .def_rw("padding1", &PhysicsUpdateContext::BodyPairQueue::mPadding1,
            // "Moved to own cache line to avoid conflicts with consumer jobs")
        // .def_rw("read_idx", &PhysicsUpdateContext::BodyPairQueue::mReadIdx,
            // "Next index to read in mBodyPair array (need to add thread index * mMaxBodyPairsPerQueue and modulo mMaxBodyPairsPerQueue)")
        // .def_rw("padding2", &PhysicsUpdateContext::BodyPairQueue::mPadding2,
            // "Moved to own cache line to avoid conflicts with producer/consumer jobs")
            ;

    nb::class_<PhysicsUpdateContext::Step> stepCls(physicsUpdateContextCls, "Step",
        "Structure that contains data needed for each collision step.");
    stepCls
        .def(nb::init<>())
        .def(nb::init<const PhysicsUpdateContext::Step &>())
        .def_rw("context", &PhysicsUpdateContext::Step::mContext,
            "The physics update context")
        .def_rw("is_first", &PhysicsUpdateContext::Step::mIsFirst,
            "If this is the first step")
        .def_rw("is_last", &PhysicsUpdateContext::Step::mIsLast,
            "If this is the last step")
        .def_rw("broad_phase_update_state", &PhysicsUpdateContext::Step::mBroadPhaseUpdateState,
            "Handle returned by Broadphase::UpdatePrepare")
        .def_rw("num_active_bodies_at_step_start", &PhysicsUpdateContext::Step::mNumActiveBodiesAtStepStart,
            "Number of bodies that were active at the start of the physics update step. Only these bodies will receive gravity (they are the first N in the active body list).")
        // .def_rw("determine_active_constraint_read_idx", &PhysicsUpdateContext::Step::mDetermineActiveConstraintReadIdx,
            // "Next constraint for determine active constraints")
        // .def_rw("padding1", &PhysicsUpdateContext::Step::mPadding1,
            // "Padding to avoid sharing cache line with the next atomic")
        // .def_rw("num_active_constraints", &PhysicsUpdateContext::Step::mNumActiveConstraints,
            // "Number of constraints in the mActiveConstraints array")
        // .def_rw("padding2", &PhysicsUpdateContext::Step::mPadding2,
            // "Padding to avoid sharing cache line with the next atomic")
        // .def_rw("setup_velocity_constraints_read_idx", &PhysicsUpdateContext::Step::mSetupVelocityConstraintsReadIdx,
        //     "Next constraint for setting up velocity constraints")
        // .def_rw("padding3", &PhysicsUpdateContext::Step::mPadding3,
        //     "Padding to avoid sharing cache line with the next atomic")
        // .def_rw("step_listener_read_idx", &PhysicsUpdateContext::Step::mStepListenerReadIdx,
        //     "Next step listener to call")
        // .def_rw("padding4", &PhysicsUpdateContext::Step::mPadding4,
        //     "Padding to avoid sharing cache line with the next atomic")
        // .def_rw("apply_gravity_read_idx", &PhysicsUpdateContext::Step::mApplyGravityReadIdx,
        //     "Next body to apply gravity to")
        // .def_rw("padding5", &PhysicsUpdateContext::Step::mPadding5,
        //     "Padding to avoid sharing cache line with the next atomic")
        // .def_rw("active_body_read_idx", &PhysicsUpdateContext::Step::mActiveBodyReadIdx,
        //     "Index of fist active body that has not yet been processed by the broadphase")
        // .def_rw("padding6", &PhysicsUpdateContext::Step::mPadding6,
        //     "Padding to avoid sharing cache line with the next atomic")
        // .def_rw("body_pair_queues", &PhysicsUpdateContext::Step::mBodyPairQueues,
            // "Queues in which to put body pairs that need to be tested by the narrowphase")
        .def_rw("max_body_pairs_per_queue", &PhysicsUpdateContext::Step::mMaxBodyPairsPerQueue,
            "Amount of body pairs that we can queue per queue")
        // .def_rw("active_find_collision_jobs", &PhysicsUpdateContext::Step::mActiveFindCollisionJobs,
        //     "A bitmask that indicates which jobs are still active")
        // .def_rw("num_body_pairs", &PhysicsUpdateContext::Step::mNumBodyPairs,
        //     "The number of body pairs found in this step (used to size the contact cache in the next step)")
        // .def_rw("num_manifolds", &PhysicsUpdateContext::Step::mNumManifolds,
        //     "The number of manifolds found in this step (used to size the contact cache in the next step)")
        // .def_rw("solve_velocity_constraints_next_island", &PhysicsUpdateContext::Step::mSolveVelocityConstraintsNextIsland,
        //     "Next island that needs to be processed for the solve velocity constraints step (doesn't need own cache line since position jobs don't run at same time)")
        // .def_rw("solve_position_constraints_next_island", &PhysicsUpdateContext::Step::mSolvePositionConstraintsNextIsland,
        //     "Next island that needs to be processed for the solve position constraints step (doesn't need own cache line since velocity jobs don't run at same time)")
        // .def_rw("integrate_velocity_read_idx", &PhysicsUpdateContext::Step::mIntegrateVelocityReadIdx,
        //     "Next active body index to take when integrating velocities")
        // .def_rw("ccd_bodies", &PhysicsUpdateContext::Step::mCCDBodies,
        //     "List of bodies that need to do continuous collision detection")
        .def_rw("ccd_bodies_capacity", &PhysicsUpdateContext::Step::mCCDBodiesCapacity,
            "Capacity of the mCCDBodies list")
        // .def_rw("num_ccd_bodies", &PhysicsUpdateContext::Step::mNumCCDBodies,
        //     "Number of CCD bodies in mCCDBodies")
        // .def_rw("next_ccd_body", &PhysicsUpdateContext::Step::mNextCCDBody,
        //     "Next unprocessed body index in mCCDBodies")
        // .def_rw("active_body_to_ccd_body", &PhysicsUpdateContext::Step::mActiveBodyToCCDBody,
        //     "A mapping between an index in BodyManager::mActiveBodies and the index in mCCDBodies")
        .def_rw("num_active_body_to_ccd_body", &PhysicsUpdateContext::Step::mNumActiveBodyToCCDBody,
            "Number of indices in mActiveBodyToCCDBody")
        .def_rw("broad_phase_prepare", &PhysicsUpdateContext::Step::mBroadPhasePrepare,
            "Prepares the new tree in the background")
        .def_rw("step_listeners", &PhysicsUpdateContext::Step::mStepListeners,
            "Listeners to notify of the beginning of a physics step")
        .def_rw("determine_active_constraints", &PhysicsUpdateContext::Step::mDetermineActiveConstraints,
            "Determine which constraints will be active during this step")
        .def_rw("apply_gravity", &PhysicsUpdateContext::Step::mApplyGravity,
            "Update velocities of bodies with gravity")
        .def_rw("find_collisions", &PhysicsUpdateContext::Step::mFindCollisions,
            "Find all collisions between active bodies an the world")
        .def_rw("update_broadphase_finalize", &PhysicsUpdateContext::Step::mUpdateBroadphaseFinalize,
            "Swap the newly built tree with the current tree")
        .def_rw("setup_velocity_constraints", &PhysicsUpdateContext::Step::mSetupVelocityConstraints,
            "Calculate properties for all constraints in the constraint manager")
        .def_rw("build_islands_from_constraints", &PhysicsUpdateContext::Step::mBuildIslandsFromConstraints,
            "Go over all constraints and assign the bodies they're attached to to an island")
        .def_rw("finalize_islands", &PhysicsUpdateContext::Step::mFinalizeIslands,
            "Finalize calculation simulation islands")
        .def_rw("body_set_island_index", &PhysicsUpdateContext::Step::mBodySetIslandIndex,
            "Set the current island index on each body (not used by the simulation, only for drawing purposes)")
        .def_rw("solve_velocity_constraints", &PhysicsUpdateContext::Step::mSolveVelocityConstraints,
            "Solve the constraints in the velocity domain")
        .def_rw("pre_integrate_velocity", &PhysicsUpdateContext::Step::mPreIntegrateVelocity,
            "Setup integration of all body positions")
        .def_rw("integrate_velocity", &PhysicsUpdateContext::Step::mIntegrateVelocity,
            "Integrate all body positions")
        .def_rw("post_integrate_velocity", &PhysicsUpdateContext::Step::mPostIntegrateVelocity,
            "Finalize integration of all body positions")
        .def_rw("resolve_ccd_contacts", &PhysicsUpdateContext::Step::mResolveCCDContacts,
            "Updates the positions and velocities for all bodies that need continuous collision detection")
        .def_rw("solve_position_constraints", &PhysicsUpdateContext::Step::mSolvePositionConstraints,
            "Solve all constraints in the position domain")
        .def_rw("contact_removed_callbacks", &PhysicsUpdateContext::Step::mContactRemovedCallbacks,
            "Calls the contact removed callbacks")
        .def_rw("soft_body_prepare", &PhysicsUpdateContext::Step::mSoftBodyPrepare,
            "Prepares updating the soft bodies")
        .def_rw("soft_body_collide", &PhysicsUpdateContext::Step::mSoftBodyCollide,
            "Finds all colliding shapes for soft bodies")
        .def_rw("soft_body_simulate", &PhysicsUpdateContext::Step::mSoftBodySimulate,
            "Simulates all particles")
        .def_rw("soft_body_finalize", &PhysicsUpdateContext::Step::mSoftBodyFinalize,
            "Finalizes the soft body update")
        .def_rw("start_next_step", &PhysicsUpdateContext::Step::mStartNextStep,
            "Job that kicks the next step (empty for the last step)");

   nb::class_<PhysicsUpdateContext::Step::CCDBody> cCDBodyCls(stepCls, "CCDBody",
        "Contains the information needed to cast a body through the scene to do continuous collision detection");
    cCDBodyCls
        .def(nb::init<BodyID, Vec3Arg, float, float>(), "body_id1"_a, "delta_position"_a, "linear_cast_threshold_sq"_a, "max_penetration"_a)
        .def_rw("delta_position", &PhysicsUpdateContext::Step::CCDBody::mDeltaPosition,
            "Desired rotation step")
        .def_rw("contact_normal", &PhysicsUpdateContext::Step::CCDBody::mContactNormal,
            "World space normal of closest hit (only valid if mFractionPlusSlop < 1)")
        .def_rw("contact_point_on2", &PhysicsUpdateContext::Step::CCDBody::mContactPointOn2,
            "World space contact point on body 2 of closest hit (only valid if mFractionPlusSlop < 1)")
        .def_rw("body_id1", &PhysicsUpdateContext::Step::CCDBody::mBodyID1,
            "Body 1 (the body that is performing collision detection)")
        .def_rw("body_id2", &PhysicsUpdateContext::Step::CCDBody::mBodyID2,
            "Body 2 (the body of the closest hit, only valid if mFractionPlusSlop < 1)")
        .def_rw("sub_shape_id2", &PhysicsUpdateContext::Step::CCDBody::mSubShapeID2,
            "Sub shape of body 2 that was hit (only valid if mFractionPlusSlop < 1)")
        .def_rw("fraction", &PhysicsUpdateContext::Step::CCDBody::mFraction,
            "Fraction at which the hit occurred")
        .def_rw("fraction_plus_slop", &PhysicsUpdateContext::Step::CCDBody::mFractionPlusSlop,
            "Fraction at which the hit occurred + extra delta to allow body to penetrate by mMaxPenetration")
        .def_rw("linear_cast_threshold_sq", &PhysicsUpdateContext::Step::CCDBody::mLinearCastThresholdSq,
            "Maximum allowed squared movement before doing a linear cast (determined by inner radius of shape)")
        .def_rw("max_penetration", &PhysicsUpdateContext::Step::CCDBody::mMaxPenetration,
            "Maximum allowed penetration (determined by inner radius of shape)")
        .def_rw("contact_settings", &PhysicsUpdateContext::Step::CCDBody::mContactSettings,
            "The contact settings for this contact");
}