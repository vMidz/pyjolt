#include "Common.h"
#include <Jolt/Physics/SoftBody/SoftBodyUpdateContext.h>
#include <Jolt/Physics/SoftBody/SoftBodyContactListener.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/Body/Body.h>

void BindSoftBodyUpdateContext(nb::module_ &m) {
    nb::class_<SoftBodyUpdateContext, NonCopyable> softBodyUpdateContextCls(m, "SoftBodyUpdateContext",
        "Temporary data used by the update of a soft body");
    softBodyUpdateContextCls
        .def_ro_static("C_VERTEX_COLLISION_BATCH", &SoftBodyUpdateContext::cVertexCollisionBatch,
            "Number of vertices to process in a batch in DetermineCollisionPlanes")
        .def_ro_static("C_VERTEX_CONSTRAINT_BATCH", &SoftBodyUpdateContext::cVertexConstraintBatch,
            "Number of vertices to group for processing batches of constraints in ApplyEdgeConstraints")
        .def_rw("body", &SoftBodyUpdateContext::mBody,
            "Body that is being updated")
        .def_rw("motion_properties", &SoftBodyUpdateContext::mMotionProperties,
            "Motion properties of that body")
        .def_rw("contact_listener", &SoftBodyUpdateContext::mContactListener,
            "Contact listener to fire callbacks to")
        .def_rw("center_of_mass_transform", &SoftBodyUpdateContext::mCenterOfMassTransform,
            "Transform of the body relative to the soft body")
        .def_rw("gravity", &SoftBodyUpdateContext::mGravity,
            "Gravity vector in local space of the soft body")
        .def_rw("displacement_due_to_gravity", &SoftBodyUpdateContext::mDisplacementDueToGravity,
            "Displacement of the center of mass due to gravity in the current time step")
        .def_rw("delta_time", &SoftBodyUpdateContext::mDeltaTime,
            "Delta time for the current time step")
        .def_rw("sub_step_delta_time", &SoftBodyUpdateContext::mSubStepDeltaTime,
            "Delta time for each sub step")
        // .def_rw("state", &SoftBodyUpdateContext::mState,                      // TODO: atomic fields
        //     "Current state of the update")
        // .def_rw("next_collision_vertex", &SoftBodyUpdateContext::mNextCollisionVertex,
        //     "Next vertex to process for DetermineCollisionPlanes")
        // .def_rw("num_collision_vertices_processed", &SoftBodyUpdateContext::mNumCollisionVerticesProcessed,
        //     "Number of vertices processed by DetermineCollisionPlanes, used to determine if we can go to the next step")
        // .def_rw("next_sensor_index", &SoftBodyUpdateContext::mNextSensorIndex,
        //     "Next sensor to process for DetermineCollisionPlanes")
        // .def_rw("num_sensors_processed", &SoftBodyUpdateContext::mNumSensorsProcessed,
        //     "Number of sensors processed by DetermineSensorCollisions, used to determine if we can go to the next step")
        // .def_rw("next_iteration", &SoftBodyUpdateContext::mNextIteration,
        //     "Next simulation iteration to process")
        // .def_rw("next_constraint_group", &SoftBodyUpdateContext::mNextConstraintGroup,
        //     "Next constraint group to process")
        // .def_rw("num_constraint_groups_processed", &SoftBodyUpdateContext::mNumConstraintGroupsProcessed,
        //     "Number of groups processed, used to determine if we can go to the next iteration")
        // .def_rw("delta_position", &SoftBodyUpdateContext::mDeltaPosition,
        //     "Delta position of the body in the current time step, should be applied after the update")
        // .def_rw("can_sleep", &SoftBodyUpdateContext::mCanSleep,
        //     "Can the body sleep? Should be applied after the update")
            ;

    nb::enum_<SoftBodyUpdateContext::EState>(softBodyUpdateContextCls, "EState",
        "Describes progress in the current update")
        .value("DETERMINE_COLLISION_PLANES", SoftBodyUpdateContext::EState::DetermineCollisionPlanes,
            "Determine collision planes for vertices in parallel")
        .value("DETERMINE_SENSOR_COLLISIONS", SoftBodyUpdateContext::EState::DetermineSensorCollisions,
            "Determine collisions with sensors in parallel")
        .value("APPLY_CONSTRAINTS", SoftBodyUpdateContext::EState::ApplyConstraints,
            "Apply constraints in parallel")
        .value("DONE", SoftBodyUpdateContext::EState::Done,
            "Update is finished");

}