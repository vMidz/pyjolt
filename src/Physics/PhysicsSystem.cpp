#include "Common.h"
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/Memory.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/SoftBody/SoftBodyContactListener.h>
#include <Jolt/Physics/PhysicsStepListener.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <nanobind/stl/vector.h>

void BindPhysicsSystem(nb::module_ &m) {
    nb::class_<PhysicsSystem, NonCopyable> physicsSystemCls(m, "PhysicsSystem",
        "The main class for the physics system. It contains all rigid bodies and simulates them.\n"
        "The main simulation is performed by the Update() call on multiple threads (if the JobSystem is configured to use them). Please refer to the general architecture overview in the Docs folder for more information.");
    physicsSystemCls
        .def(nb::init<>(),
            "Constructor / Destructor")
        .def("init", &PhysicsSystem::Init,
            "max_bodies"_a, "num_body_mutexes"_a, "max_body_pairs"_a, "max_contact_constraints"_a,
            "broad_phase_layer_interface"_a, "object_vs_broad_phase_layer_filter"_a, "object_layer_pair_filter"_a,
            "Initialize the system.\n"
            "Args:\n"
            "    max_bodies (int): Maximum number of bodies to support.\n"
            "    num_body_mutexes (int): Number of body mutexes to use. Should be a power of 2 in the range [1, 64], use 0 to auto detect.\n"
            "    max_body_pairs (int): Maximum amount of body pairs to process (anything else will fall through the world), this number should generally be much higher than the max amount of contact points as there will be lots of bodies close that are not actually touching.\n"
            "    max_contact_constraints (int): Maximum amount of contact constraints to process (anything else will fall through the world).\n"
            "    broad_phase_layer_interface (BroadPhaseLayerInterface): Information on the mapping of object layers to broad phase layers. Since this is a virtual interface, the instance needs to stay alive during the lifetime of the PhysicsSystem.\n"
            "    object_vs_broad_phase_layer_filter (ObjectVsBroadPhaseLayerFilter): Filter callback function that is used to determine if an object layer collides with a broad phase layer. Since this is a virtual interface, the instance needs to stay alive during the lifetime of the PhysicsSystem.\n"
            "    object_layer_pair_filter (ObjectLayerPairFilter): Filter callback function that is used to determine if two object layers collide. Since this is a virtual interface, the instance needs to stay alive during the lifetime of the PhysicsSystem.")
        .def("set_body_activation_listener", &PhysicsSystem::SetBodyActivationListener, "listener"_a.none(),
            "Listener that is notified whenever a body is activated/deactivated")
        .def("get_body_activation_listener", &PhysicsSystem::GetBodyActivationListener, nb::rv_policy::reference)
        .def("set_contact_listener", &PhysicsSystem::SetContactListener, "listener"_a.none(),
            "Listener that is notified whenever a contact point between two bodies is added/updated/removed.\n"
            "You can't change contact listener during PhysicsSystem::Update but it can be changed at any other time.")
        .def("get_contact_listener", &PhysicsSystem::GetContactListener, nb::rv_policy::reference)
        .def("set_soft_body_contact_listener", &PhysicsSystem::SetSoftBodyContactListener, "listener"_a,
            "Listener that is notified whenever a contact point between a soft body and another body")
        .def("get_soft_body_contact_listener", &PhysicsSystem::GetSoftBodyContactListener, nb::rv_policy::reference)
        // .def("set_combine_friction", &PhysicsSystem::SetCombineFriction, "combine_friction"_a,
            // "Set the function that combines the friction of two bodies and returns it\n"
            // "Default method is the geometric mean: sqrt(friction1 * friction2).")
        // .def("get_combine_friction", &PhysicsSystem::GetCombineFriction)
        // .def("set_combine_restitution", &PhysicsSystem::SetCombineRestitution, "combine_restition"_a,
            // "Set the function that combines the restitution of two bodies and returns it\n"
            // "Default method is max(restitution1, restitution1)")
        .def("get_combine_restitution", &PhysicsSystem::GetCombineRestitution)
        .def("set_physics_settings", &PhysicsSystem::SetPhysicsSettings, "settings"_a,
            "Control the main constants of the physics simulation")
        .def("get_physics_settings", &PhysicsSystem::GetPhysicsSettings)
        .def("get_body_interface", nb::overload_cast<>(&PhysicsSystem::GetBodyInterface), nb::rv_policy::reference_internal,
            "Access to the body interface. This interface allows to to create / remove bodies and to change their properties.")
        .def("get_body_interface_no_lock", nb::overload_cast<>(&PhysicsSystem::GetBodyInterfaceNoLock),
            "Version that does not lock the bodies, use with great care!")
        .def("get_broad_phase_query", &PhysicsSystem::GetBroadPhaseQuery, nb::rv_policy::reference,
            "Access to the broadphase interface that allows coarse collision queries")
        .def("get_narrow_phase_query", &PhysicsSystem::GetNarrowPhaseQuery, nb::rv_policy::reference,
            "Interface that allows fine collision queries against first the broad phase and then the narrow phase.")
        .def("get_narrow_phase_query_no_lock", &PhysicsSystem::GetNarrowPhaseQueryNoLock, nb::rv_policy::reference)
        .def("add_constraint", &PhysicsSystem::AddConstraint, "constraint"_a,
            "Add constraint to the world")
        .def("remove_constraint", &PhysicsSystem::RemoveConstraint, "constraint"_a,
            "Remove constraint from the world")
        // .def("add_constraints", &PhysicsSystem::AddConstraints, "constraints"_a, "number"_a,
            // "Batch add constraints.")
        // .def("remove_constraints", &PhysicsSystem::RemoveConstraints, "constraints"_a, "number"_a,
            // "Batch remove constraints.")
        .def("get_constraints", &PhysicsSystem::GetConstraints,
            "Get a list of all constraints")
        .def("optimize_broad_phase", &PhysicsSystem::OptimizeBroadPhase,
            "Optimize the broadphase, needed only if you've added many bodies prior to calling Update() for the first time.\n"
            "Don't call this every frame as PhysicsSystem::Update spreads out the same work over multiple frames.\n"
            "If you add many bodies through BodyInterface::AddBodiesPrepare/AddBodiesFinalize and if the bodies in a batch are\n"
            "in a roughly unoccupied space (e.g. a new level section) then a call to OptimizeBroadPhase is also not needed\n"
            "as batch adding creates an efficient bounding volume hierarchy.\n"
            "Don't call this function while bodies are being modified from another thread or use the locking BodyInterface to modify bodies.")
        .def("add_step_listener", &PhysicsSystem::AddStepListener, "listener"_a,
            "Adds a new step listener")
        .def("remove_step_listener", &PhysicsSystem::RemoveStepListener, "listener"_a,
            "Removes a step listener")
        .def("update", &PhysicsSystem::Update, "delta_time"_a, "collision_steps"_a, "temp_allocator"_a, "job_system"_a,
            nb::call_guard<nb::gil_scoped_release>(),
            "Simulate the system.\n"
            "The world steps for a total of inDeltaTime seconds. This is divided in inCollisionSteps iterations.\n"
            "Each iteration consists of collision detection followed by an integration step.\n"
            "This function internally spawns jobs using inJobSystem and waits for them to complete, so no jobs will be running when this function returns.")
        .def("save_state", &PhysicsSystem::SaveState, "stream"_a, "state"_a = (int)EStateRecorderState::All, "filter"_a = nullptr,
            "Saving state for replay")
        .def("restore_state", &PhysicsSystem::RestoreState, "stream"_a, "filter"_a = nullptr,
            "Restoring state for replay. Returns false if failed.")
        .def("save_body_state", &PhysicsSystem::SaveBodyState, "body"_a, "stream"_a,
            "Saving state of a single body.")
        .def("restore_body_state", &PhysicsSystem::RestoreBodyState, "body"_a, "stream"_a,
            "Restoring state of a single body.")
        .def("draw_bodies", &PhysicsSystem::DrawBodies, "settings"_a, "renderer"_a, "body_filter"_a = nullptr,
            "Draw the state of the bodies (debugging purposes)")
        .def("draw_constraints", &PhysicsSystem::DrawConstraints, "renderer"_a,
            "Draw the constraints only (debugging purposes)")
        .def("draw_constraint_limits", &PhysicsSystem::DrawConstraintLimits, "renderer"_a,
            "Draw the constraint limits only (debugging purposes)")
        .def("draw_constraint_reference_frame", &PhysicsSystem::DrawConstraintReferenceFrame, "renderer"_a,
            "Draw the constraint reference frames only (debugging purposes)")
        .def("set_gravity", &PhysicsSystem::SetGravity, "gravity"_a,
            "Set gravity value")
        .def("get_gravity", &PhysicsSystem::GetGravity)
        .def("get_body_lock_interface_no_lock", &PhysicsSystem::GetBodyLockInterfaceNoLock,
            "Returns a locking interface that won't actually lock the body. Use with great care!")
        .def("get_body_lock_interface", &PhysicsSystem::GetBodyLockInterface,
            "Returns a locking interface that locks the body so other threads cannot modify it.")
        .def("get_default_broad_phase_layer_filter", &PhysicsSystem::GetDefaultBroadPhaseLayerFilter, "layer"_a,
            "Get an broadphase layer filter that uses the default pair filter and a specified object layer to determine if broadphase layers collide")
        .def("get_default_layer_filter", &PhysicsSystem::GetDefaultLayerFilter, "layer"_a,
            "Get an object layer filter that uses the default pair filter and a specified layer to determine if layers collide")
        .def("get_num_bodies", &PhysicsSystem::GetNumBodies,
            "Gets the current amount of bodies that are in the body manager")
        .def("get_num_active_bodies", &PhysicsSystem::GetNumActiveBodies, "type"_a,
            "Gets the current amount of active bodies that are in the body manager")
        .def("get_max_bodies", &PhysicsSystem::GetMaxBodies,
            "Get the maximum amount of bodies that this physics system supports")
        .def("get_body_stats", &PhysicsSystem::GetBodyStats,
            "Get stats about the bodies in the body manager (slow, iterates through all bodies)")
        .def("get_bodies", [](PhysicsSystem &self){
            BodyIDVector outBodyIDs;
            self.GetBodies(outBodyIDs);
            return outBodyIDs;
        }, "Get copy of the list of all bodies under protection of a lock.\n"
            "Returns:\n"
            "    BodyIDVector: On return, this will contain the list of BodyIDs.")
        .def("get_active_bodies", &PhysicsSystem::GetActiveBodies, "type"_a, "body_i_ds"_a,
            "Get copy of the list of active bodies under protection of a lock.\n"
            "Args:\n"
            "    type (EBodyType): The type of bodies to get."
            "    body_i_ds (BodyIDVector): On return, this will contain the list of BodyIDs.\n")
        .def("get_active_bodies_unsafe", &PhysicsSystem::GetActiveBodiesUnsafe, "type"_a, nb::rv_policy::reference,
            "Get the list of active bodies, use GetNumActiveBodies() to find out how long the list is.\n"
            "Note: Not thread safe. The active bodies list can change at any moment when other threads are doing work. Use GetActiveBodies() if you need a thread safe version.")
        .def("were_bodies_in_contact", &PhysicsSystem::WereBodiesInContact, "body1_id"_a, "body2_id"_a,
            "Check if 2 bodies were in contact during the last simulation step. Since contacts are only detected between active bodies, so at least one of the bodies must be active in order for this function to work.\n"
            "It queries the state at the time of the last PhysicsSystem::Update and will return true if the bodies were in contact, even if one of the bodies was moved / removed afterwards.\n"
            "This function can be called from any thread when the PhysicsSystem::Update is not running. During PhysicsSystem::Update this function is only valid during contact callbacks:\n"
            "- During the ContactListener::OnContactAdded callback this function can be used to determine if a different contact pair between the bodies was active in the previous simulation step (function returns true) or if this is the first step that the bodies are touching (function returns false).\n"
            "- During the ContactListener::OnContactRemoved callback this function can be used to determine if this is the last contact pair between the bodies (function returns false) or if there are other contacts still present (function returns true).")
        .def("get_bounds", &PhysicsSystem::GetBounds,
            "Get the bounding box of all bodies in the physics system")
        // .def("report_broadphase_stats", &PhysicsSystem::ReportBroadphaseStats,
            // "Trace the accumulated broadphase stats to the TTY")
        .def_rw_static("draw_motion_quality_linear_cast", &PhysicsSystem::sDrawMotionQualityLinearCast,
            "Draw debug info for objects that perform continuous collision detection through the linear cast motion quality");
}