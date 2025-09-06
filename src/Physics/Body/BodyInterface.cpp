#include "Common.h"
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Constraints/TwoBodyConstraint.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>

void BindBodyInterface(nb::module_ &m) {
    nb::class_<BodyInterface, NonCopyable>(m, "BodyInterface",
        "Class that provides operations on bodies using a body ID. Note that if you need to do multiple operations on a single body, it is more efficient to lock the body once and combine the operations.\n"
        "All quantities are in world space unless otherwise specified.")

        .def("init", &BodyInterface::Init,
            "body_lock_interface"_a, "body_manager"_a, "broad_phase"_a,
            "Initialize the interface (should only be called by PhysicsSystem)")

        .def("create_body", &BodyInterface::CreateBody,
            "settings"_a, nb::rv_policy::reference_internal,
            "Create a rigid body.\n"
            "Returns:\n"
            "    Body*: Created body or null when out of bodies.")

        .def("create_soft_body", &BodyInterface::CreateSoftBody,
            "settings"_a, nb::rv_policy::reference_internal,
            "Create a soft body.\n"
            "Returns:\n"
            "    Body*: Created body or null when out of bodies.")

        .def("create_body_with_id", &BodyInterface::CreateBodyWithID,
            "body_id"_a, "settings"_a, nb::rv_policy::reference_internal,
            "Create a rigid body with specified ID. This function can be used if a simulation is to run in sync between clients or if a simulation needs to be restored exactly.\n"
            "The ID created on the server can be replicated to the client and used to create a deterministic simulation.\n"
            "Returns:\n"
            "    Body*: Created body or null when the body ID is invalid or a body of the same ID already exists.")

        .def("create_soft_body_with_id", &BodyInterface::CreateSoftBodyWithID,
            "body_id"_a, "settings"_a, nb::rv_policy::reference_internal,
            "Create a soft body with specified ID. See comments at CreateBodyWithID.")

        .def("create_body_without_id", &BodyInterface::CreateBodyWithoutID,
            "settings"_a, nb::rv_policy::reference_internal,
            "Advanced use only. Creates a rigid body without specifying an ID. This body cannot be added to the physics system until it has been assigned a body ID.\n"
            "This can be used to decouple allocation from registering the body. A call to CreateBodyWithoutID followed by AssignBodyID is equivalent to calling CreateBodyWithID.\n"
            "Returns:\n"
            "    Body*: Created body.")

        .def("create_soft_body_without_id", &BodyInterface::CreateSoftBodyWithoutID,
            "settings"_a, nb::rv_policy::reference_internal,
            "Advanced use only. Creates a body without specifying an ID. See comments at CreateBodyWithoutID.")

        .def("destroy_body_without_id", &BodyInterface::DestroyBodyWithoutID,
            "body_id"_a,
            "Advanced use only. Destroy a body previously created with CreateBodyWithoutID that hasn't gotten an ID yet through the AssignBodyID function,\n"
            "or a body that has had its body ID unassigned through UnassignBodyIDs. Bodies that have an ID should be destroyed through DestroyBody.")

        .def("assign_body_id", nb::overload_cast<Body *>(&BodyInterface::AssignBodyID),
            "body_id"_a,
            "Advanced use only. Assigns the next available body ID to a body that was created using CreateBodyWithoutID. After this call, the body can be added to the physics system.\n"
            "Returns:\n"
            "    bool: false if the body already has an ID or out of body ids.")

        .def("assign_body_id", nb::overload_cast<Body *, const BodyID &>(&BodyInterface::AssignBodyID),
            "body"_a, "body_id"_a,
            "Advanced use only. Assigns a body ID to a body that was created using CreateBodyWithoutID. After this call, the body can be added to the physics system.\n"
            "Returns:\n"
            "    bool: false if the body already has an ID or if the ID is not valid.")

        .def("unassign_body_id", &BodyInterface::UnassignBodyID,
            "body_id"_a, nb::rv_policy::reference_internal,
            "Advanced use only. See UnassignBodyIDs. Unassigns the ID of a single body.")

        .def("unassign_body_ids", [](BodyInterface &self, Array<BodyID> &bodyIds) {
            Array<Body *> outBodies(bodyIds.size());
            if (!bodyIds.empty())
                self.UnassignBodyIDs(bodyIds.data(), (int)bodyIds.size(), outBodies.data());

        }, "body_id"_a,
            "Advanced use only. Removes a number of body IDs from their bodies and returns the body pointers. Before calling this, the body should have been removed from the physics system.\n"
            "The body can be destroyed through DestroyBodyWithoutID. This can be used to decouple deallocation. A call to UnassignBodyIDs followed by calls to DestroyBodyWithoutID is equivalent to calling DestroyBodies.\n"
            "Args:\n"
            "    body_i_ds (BodyID*): A list of body IDs.\n"
            "    number (int): Number of bodies in the list.\n"
            "    bodies (**): If not null on input, this will contain a list of body pointers corresponding to inBodyIDs that can be destroyed afterwards (caller assumes ownership over these).")

        .def("destroy_body", &BodyInterface::DestroyBody,
            "body_id"_a,
            "Destroy a body.\n"
            "Make sure that you remove the body from the physics system using BodyInterface::RemoveBody before calling this function.")

        .def("destroy_bodies", [](BodyInterface &self, Array <BodyID> bodyIds) {
            if (!bodyIds.empty())
                self.DestroyBodies(bodyIds.data(), bodyIds.size());
        }, "body_id"_a,
           "Destroy multiple bodies\n"
           "Make sure that you remove the bodies from the physics system using BodyInterface::RemoveBody before calling this function.")

        .def("add_body", &BodyInterface::AddBody, "body_id"_a, "inActivationMode"_a,
             "Add body to the physics system.\n"
             "Note that if you need to add multiple bodies, use the AddBodiesPrepare/AddBodiesFinalize function.\n"
             "Adding many bodies, one at a time, results in a really inefficient broadphase until PhysicsSystem::OptimizeBroadPhase is called or when PhysicsSystem::Update rebuilds the tree!\n"
             "After adding, to get a body by ID use the BodyLockRead or BodyLockWrite interface!")

        .def("remove_body", &BodyInterface::RemoveBody, "body_id"_a, "Remove body from the physics system.")
        .def("is_added", &BodyInterface::IsAdded, "body_id"_a, "Check if a body has been added to the physics system.")

        .def("create_and_add_body", &BodyInterface::CreateAndAddBody,
            "settings"_a, "activation_mode"_a,
            "Combines CreateBody and AddBody.\n"
            "Returns:\n"
            "    BodyID: Created body ID or an invalid ID when out of bodies.")

        .def("create_and_add_soft_body", &BodyInterface::CreateAndAddSoftBody,
            "settings"_a, "activation_mode"_a,
            "Combines CreateSoftBody and AddBody.\n"
            "Returns:\n"
            "    BodyID: Created body ID or an invalid ID when out of bodies.")

        .def("add_bodies_prepare", &BodyInterface::AddBodiesPrepare, "bodies"_a, "number"_a,
             "Prepare adding inNumber bodies at ioBodies to the PhysicsSystem, returns a handle that should be used in AddBodiesFinalize/Abort.\n"
             "This can be done on a background thread without influencing the PhysicsSystem.\n"
             "ioBodies may be shuffled around by this function and should be kept that way until AddBodiesFinalize/Abort is called.")

        .def("add_bodies_finalize", &BodyInterface::AddBodiesFinalize,
            "bodies"_a, "number"_a, "add_state"_a, "activation_mode"_a,
            "Finalize adding bodies to the PhysicsSystem, supply the return value of AddBodiesPrepare in inAddState.\n"
            "Please ensure that the ioBodies array passed to AddBodiesPrepare is unmodified and passed again to this function.")

        .def("add_bodies_abort", &BodyInterface::AddBodiesAbort, "bodies"_a, "number"_a, "add_state"_a,
             "Abort adding bodies to the PhysicsSystem, supply the return value of AddBodiesPrepare in inAddState.\n"
             "This can be done on a background thread without influencing the PhysicsSystem.\n"
             "Please ensure that the ioBodies array passed to AddBodiesPrepare is unmodified and passed again to this function.")

        .def("remove_bodies", &BodyInterface::RemoveBodies,
            "bodies"_a, "number"_a,
            "Remove inNumber bodies in ioBodies from the PhysicsSystem.\n"
            "ioBodies may be shuffled around by this function.")

        .def("activate_body", &BodyInterface::ActivateBody,
            "body_id"_a)
        .def("activate_bodies", [](BodyInterface &self, Array<BodyID> &bodyIds) {
            if (!bodyIds.empty())
                self.ActivateBodies(bodyIds.data(), (int)bodyIds.size());
        }, "in_body_i_ds"_a)

        .def("activate_bodies_in_aa_box", &BodyInterface::ActivateBodiesInAABox,
            "box"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a)
        .def("deactivate_body", &BodyInterface::DeactivateBody, "body_id"_a)
        .def("deactivate_bodies", [](BodyInterface &self, Array<BodyID> &bodyIds) {
            if (!bodyIds.empty()) {
                self.DeactivateBodies(bodyIds.data(), (int)bodyIds.size());
            }
        }, "body_ids"_a)
        .def("is_active", &BodyInterface::IsActive, "body_id"_a)
        .def("reset_sleep_timer", &BodyInterface::ResetSleepTimer, "body_id"_a)

        .def("create_constraint", &BodyInterface::CreateConstraint,
            "settings"_a, "body_id1"_a, "body_id2"_a, nb::rv_policy::reference_internal, "Create a two body constraint")
        .def("activate_constraint", &BodyInterface::ActivateConstraint,
            "constraint"_a, "Activate non-static bodies attached to a constraint")

        .def("get_shape", [](const BodyInterface &self, const BodyID &bodyID) -> const Shape * {
            RefConst<Shape> shapeRef = self.GetShape(bodyID);
            return shapeRef.GetPtr();
        }, "body_id"_a, nb::rv_policy::reference_internal,
          "Get the current shape")
        .def("set_shape", &BodyInterface::SetShape,
             "body_id"_a, "shape"_a, "update_mass_properties"_a, "activation_mode"_a,
            "Set a new shape on the body.\n"
            "Args:\n"
            "    body_id (**): Body ID of body that had its shape changed.\n"
            "    shape (Shape*): The new shape.\n"
            "    update_mass_properties (bool): When true, the mass and inertia tensor is recalculated.\n"
            "    activation_mode (EActivation): Weather or not to activate the body.")

        .def("notify_shape_changed", &BodyInterface::NotifyShapeChanged,
            "body_id"_a, "previous_center_of_mass"_a, "update_mass_properties"_a, "activation_mode"_a,
            "Notify all systems to indicate that a shape has changed (usable for MutableCompoundShapes).\n"
            "Args:\n"
            "    body_id (BodyId): Body ID of body that had its shape changed.\n"
            "    previous_center_of_mass (Vec3): Center of mass of the shape before the alterations.\n"
            "    update_mass_properties (bool): When true, the mass and inertia tensor is recalculated.\n"
            "    activation_mode (EActivation): Weather or not to activate the body.")

        .def("set_object_layer", &BodyInterface::SetObjectLayer,
            "body_id"_a, "inLayer"_a)

        .def("get_object_layer", &BodyInterface::GetObjectLayer,
            "body_id"_a)

        .def("set_position_and_rotation", &BodyInterface::SetPositionAndRotation,
            "body_id"_a, "inPosition"_a, "inRotation"_a, "inActivationMode"_a,
            "Position and rotation of a body")

        .def("set_position_and_rotation_when_changed", &BodyInterface::SetPositionAndRotationWhenChanged,
            "body_id"_a, "inPosition"_a, "inRotation"_a, "inActivationMode"_a)

        .def("get_position_and_rotation", [](const BodyInterface &self, const BodyID &bodyID) {
            RVec3 pos;
            Quat rot;
            self.GetPositionAndRotation(bodyID, pos, rot);
            return nb::make_tuple(pos, rot);
        }, "body_id"_a,
           "Get body position (RVec3) and rotation (Quat).")
        .def("set_position", &BodyInterface::SetPosition, "body_id"_a, "inPosition"_a, "inActivationMode"_a)
        .def("get_position", &BodyInterface::GetPosition, "body_id"_a)
        .def("get_center_of_mass_position", &BodyInterface::GetCenterOfMassPosition, "body_id"_a)
        .def("set_rotation", &BodyInterface::SetRotation, "body_id"_a, "inRotation"_a, "inActivationMode"_a)
        .def("get_rotation", &BodyInterface::GetRotation, "body_id"_a)
        .def("get_world_transform", &BodyInterface::GetWorldTransform, "body_id"_a)
        .def("get_center_of_mass_transform", &BodyInterface::GetCenterOfMassTransform, "body_id"_a)

        .def("move_kinematic", &BodyInterface::MoveKinematic,
            "body_id"_a, "target_position"_a, "target_rotation"_a, "delta_time"_a,
            "Set velocity of body such that it will be positioned at inTargetPosition/Rotation in inDeltaTime seconds (will activate body if needed)")

        .def("set_linear_and_angular_velocity", &BodyInterface::SetLinearAndAngularVelocity,
            "body_id"_a, "linear_velocity"_a, "angular_velocity"_a,
            "Linear or angular velocity (functions will activate body if needed).\n"
            "Note that the linear velocity is the velocity of the center of mass, which may not coincide with the position of your object, to correct for this: \\f$VelocityCOM = Velocity - AngularVelocity \\times ShapeCOM\\f$")

        .def("get_linear_and_angular_velocity", [](const BodyInterface &self, const BodyID &bodyID) {
            Vec3 lin_vel;
            Vec3 ang_vel;
            self.GetLinearAndAngularVelocity(bodyID, lin_vel, ang_vel);
            return nb::make_tuple(lin_vel, ang_vel);
        }, "body_id"_a,
           "Get body linear (Vec3) and angular (Vec3) velocity.")
        .def("set_linear_velocity", &BodyInterface::SetLinearVelocity, "body_id"_a, "inLinearVelocity"_a)
        .def("get_linear_velocity", &BodyInterface::GetLinearVelocity, "body_id"_a)
        .def("add_linear_velocity", &BodyInterface::AddLinearVelocity, "body_id"_a, "inLinearVelocity"_a)
        .def("add_linear_and_angular_velocity", &BodyInterface::AddLinearAndAngularVelocity, "body_id"_a, "inLinearVelocity"_a, "inAngularVelocity"_a)
        .def("set_angular_velocity", &BodyInterface::SetAngularVelocity, "body_id"_a, "inAngularVelocity"_a)
        .def("get_angular_velocity", &BodyInterface::GetAngularVelocity, "body_id"_a)
        .def("get_point_velocity", &BodyInterface::GetPointVelocity, "body_id"_a, "inPoint"_a)
        .def("set_position_rotation_and_velocity", &BodyInterface::SetPositionRotationAndVelocity,
             "body_id"_a, "position"_a, "rotation"_a, "linear_velocity"_a, "angular_velocity"_a,
             "Set the complete motion state of a body.\n"
             "Note that the linear velocity is the velocity of the center of mass, which may not coincide with the position of your object, to correct for this: \\f$VelocityCOM = Velocity - AngularVelocity \\times ShapeCOM\\f$")

        .def("add_force", nb::overload_cast<const BodyID &, Vec3Arg, EActivation>(&BodyInterface::AddForce),
            "body_id"_a, "force"_a, "activation_mode"_a = EActivation::Activate,
            "Add forces to the body")

        .def("add_force", nb::overload_cast<const BodyID &, Vec3Arg, RVec3Arg, EActivation>(&BodyInterface::AddForce),
            "body_id"_a, "force"_a, "point"_a, "activation_mode"_a = EActivation::Activate)

        .def("add_torque", &BodyInterface::AddTorque,
            "body_id"_a, "torque"_a, "activation_mode"_a = EActivation::Activate)

        .def("add_force_and_torque", &BodyInterface::AddForceAndTorque, "body_id"_a, "force"_a, "torque"_a, "activation_mode"_a = EActivation::Activate,
            "See Body::AddTorque")

        .def("add_impulse", nb::overload_cast<const BodyID &, Vec3Arg>(&BodyInterface::AddImpulse),
             "body_id"_a, "impulse"_a,
             "Add impulse at center of mass.")

        .def("add_impulse", nb::overload_cast<const BodyID &, Vec3Arg, RVec3Arg>(&BodyInterface::AddImpulse),
             "body_id"_a, "impulse"_a, "point"_a,
             "Add impulse at a specific point.")
        .def("add_angular_impulse", &BodyInterface::AddAngularImpulse, "body_id"_a, "in_angular_impulse"_a)

        .def("apply_buoyancy_impulse", &BodyInterface::ApplyBuoyancyImpulse, "body_id"_a, "in_surface_position"_a,
            "surface_normal"_a, "buoyancy"_a, "linear_drag"_a, "angular_drag"_a, "fluid_velocity"_a, "gravity"_a, "delta_time"_a)

        .def("get_body_type", &BodyInterface::GetBodyType, "body_id"_a)

        .def("set_motion_type", &BodyInterface::SetMotionType, "body_id"_a, "motion_type"_a, "activation_mode"_a)
        .def("get_motion_type", &BodyInterface::GetMotionType, "body_id"_a)

        .def("set_motion_quality", &BodyInterface::SetMotionQuality, "body_id"_a, "motion_quality"_a)
        .def("get_motion_quality", &BodyInterface::GetMotionQuality, "body_id"_a)

        .def("get_inverse_inertia", &BodyInterface::GetInverseInertia, "body_id"_a, "Get inverse inertia tensor in world space")

        .def("set_restitution", &BodyInterface::SetRestitution, "body_id"_a, "restitution"_a)
        .def("get_restitution", &BodyInterface::GetRestitution, "body_id"_a)

        .def("set_friction", &BodyInterface::SetFriction, "body_id"_a, "friction"_a)
        .def("get_friction", &BodyInterface::GetFriction, "body_id"_a)

        .def("set_gravity_factor", &BodyInterface::SetGravityFactor, "body_id"_a, "gravity_factor"_a)
        .def("get_gravity_factor", &BodyInterface::GetGravityFactor, "body_id"_a)

        .def("set_use_manifold_reduction", &BodyInterface::SetUseManifoldReduction,
             "body_id"_a, "in_use_reduction"_a)

        .def("get_use_manifold_reduction", &BodyInterface::GetUseManifoldReduction, "body_id"_a)

        .def("get_transformed_shape", &BodyInterface::GetTransformedShape,
             "body_id"_a,
             "Get transform and shape for this body, used to perform collision detection")

        .def("get_user_data", &BodyInterface::GetUserData, "Get the user data for a body")
        .def("set_user_data", &BodyInterface::SetUserData, "Set the user data for a body", "body_id"_a, "user_data"_a)

        .def("get_material", &BodyInterface::GetMaterial,
             "body_id"_a, "in_sub_shape_id"_a, nb::rv_policy::reference_internal,
             "Get the material for a particular sub shape")

        .def("invalidate_contact_cache", &BodyInterface::InvalidateContactCache, "body_id"_a,
             "Set the Body::EFlags::InvalidateContactCache flag for the specified body. This means that the collision cache is invalid for any body pair involving that body until the next physics step.");
}
