#include "Common.h"
#include "Jolt/Physics/PhysicsSettings.h"

void BindPhysicsSettings(nb::module_ &m) {
    m.attr("DEFAULT_COLLISION_TOLERANCE") = cDefaultCollisionTolerance;
    m.attr("DEFAULT_PENETRATION_TOLERANCE") = cDefaultPenetrationTolerance;
    m.attr("DEFAULT_CONVEX_RADIUS") = cDefaultConvexRadius;
    m.attr("CAPSULE_PROJECTION_SLOP") = cCapsuleProjectionSlop;
    m.attr("MAX_PHYSICS_JOBS") = cMaxPhysicsJobs;
    m.attr("MAX_PHYSICS_BARRIERS") = cMaxPhysicsBarriers;

    nb::class_<PhysicsSettings> physicsSettingsCls(m, "PhysicsSettings");
    physicsSettingsCls
        .def_rw("max_in_flight_body_pairs", &PhysicsSettings::mMaxInFlightBodyPairs,
            "Size of body pairs array, corresponds to the maximum amount of potential body pairs that can be in flight at any time.\n"
            "Setting this to a low value will use less memory but slow down simulation as threads may run out of narrow phase work.")
        .def_rw("step_listeners_batch_size", &PhysicsSettings::mStepListenersBatchSize,
            "How many PhysicsStepListeners to notify in 1 batch")
        .def_rw("step_listener_batches_per_job", &PhysicsSettings::mStepListenerBatchesPerJob,
            "How many step listener batches are needed before spawning another job (set to INT_MAX if no parallelism is desired)")
        .def_rw("baumgarte", &PhysicsSettings::mBaumgarte,
            "Baumgarte stabilization factor (how much of the position error to 'fix' in 1 update) (unit: dimensionless, 0 = nothing, 1 = 100%)")
        .def_rw("speculative_contact_distance", &PhysicsSettings::mSpeculativeContactDistance,
            "Radius around objects inside which speculative contact points will be detected. Note that if this is too big\n"
            "you will get ghost collisions as speculative contacts are based on the closest points during the collision detection\n"
            "step which may not be the actual closest points by the time the two objects hit (unit: meters)")
        .def_rw("penetration_slop", &PhysicsSettings::mPenetrationSlop,
            "How much bodies are allowed to sink into each other (unit: meters)")
        .def_rw("linear_cast_threshold", &PhysicsSettings::mLinearCastThreshold,
            "Fraction of its inner radius a body must move per step to enable casting for the LinearCast motion quality")
        .def_rw("linear_cast_max_penetration", &PhysicsSettings::mLinearCastMaxPenetration,
            "Fraction of its inner radius a body may penetrate another body for the LinearCast motion quality")
        .def_rw("manifold_tolerance_sq", &PhysicsSettings::mManifoldToleranceSq,
            "Max squared distance to use to determine if two points are on the same plane for determining the contact manifold between two shape faces (unit: meter^2)")
        .def_rw("max_penetration_distance", &PhysicsSettings::mMaxPenetrationDistance,
            "Maximum distance to correct in a single iteration when solving position constraints (unit: meters)")
        .def_rw("body_pair_cache_max_delta_position_sq", &PhysicsSettings::mBodyPairCacheMaxDeltaPositionSq,
            "Maximum relative delta position for body pairs to be able to reuse collision results from last frame (units: meter^2)")
        .def_rw("body_pair_cache_cos_max_delta_rotation_div2", &PhysicsSettings::mBodyPairCacheCosMaxDeltaRotationDiv2,
            "Maximum relative delta orientation for body pairs to be able to reuse collision results from last frame, stored as cos(max angle / 2)")
        .def_rw("contact_normal_cos_max_delta_rotation", &PhysicsSettings::mContactNormalCosMaxDeltaRotation,
            "Maximum angle between normals that allows manifolds between different sub shapes of the same body pair to be combined")
        .def_rw("contact_point_preserve_lambda_max_dist_sq", &PhysicsSettings::mContactPointPreserveLambdaMaxDistSq,
            "Maximum allowed distance between old and new contact point to preserve contact forces for warm start (units: meter^2)")
        .def_rw("num_velocity_steps", &PhysicsSettings::mNumVelocitySteps,
            "Number of solver velocity iterations to run\n"
            "Note that this needs to be >= 2 in order for friction to work (friction is applied using the non-penetration impulse from the previous iteration)")
        .def_rw("num_position_steps", &PhysicsSettings::mNumPositionSteps,
            "Number of solver position iterations to run")
        .def_rw("min_velocity_for_restitution", &PhysicsSettings::mMinVelocityForRestitution,
            "Minimal velocity needed before a collision can be elastic (unit: m)")
        .def_rw("time_before_sleep", &PhysicsSettings::mTimeBeforeSleep,
            "Time before object is allowed to go to sleep (unit: seconds)")
        .def_rw("point_velocity_sleep_threshold", &PhysicsSettings::mPointVelocitySleepThreshold,
            "Velocity of points on bounding box of object below which an object can be considered sleeping (unit: m/s)")
        .def_rw("deterministic_simulation", &PhysicsSettings::mDeterministicSimulation,
            "By default the simulation is deterministic, it is possible to turn this off by setting this setting to false. This will make the simulation run faster but it will no longer be deterministic.")
        .def_rw("constraint_warm_start", &PhysicsSettings::mConstraintWarmStart,
            "Whether or not to use warm starting for constraints (initially applying previous frames impulses)")
        .def_rw("use_body_pair_contact_cache", &PhysicsSettings::mUseBodyPairContactCache,
            "Whether or not to use the body pair cache, which removes the need for narrow phase collision detection when orientation between two bodies didn't change")
        .def_rw("use_manifold_reduction", &PhysicsSettings::mUseManifoldReduction,
            "Whether or not to reduce manifolds with similar contact normals into one contact manifold (see description at Body::SetUseManifoldReduction)")
        .def_rw("use_large_island_splitter", &PhysicsSettings::mUseLargeIslandSplitter,
            "If we split up large islands into smaller parallel batches of work (to improve performance)")
        .def_rw("allow_sleeping", &PhysicsSettings::mAllowSleeping,
            "If objects can go to sleep or not")
        .def_rw("check_active_edges", &PhysicsSettings::mCheckActiveEdges,
            "When false, we prevent collision against non-active (shared) edges. Mainly for debugging the algorithm.");
}