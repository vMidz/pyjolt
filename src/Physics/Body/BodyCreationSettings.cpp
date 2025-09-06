#include "Common.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/unordered_map.h>

void BindBodyCreationSettings(nb::module_ &m) {
    nb::enum_<EOverrideMassProperties>(m, "EOverrideMassProperties",
        "Enum used in BodyCreationSettings to indicate how mass and inertia should be calculated")
        .value("CALCULATE_MASS_AND_INERTIA", EOverrideMassProperties::CalculateMassAndInertia,
            "Tells the system to calculate the mass and inertia based on density")
        .value("CALCULATE_INERTIA", EOverrideMassProperties::CalculateInertia,
            "Tells the system to take the mass from mMassPropertiesOverride and to calculate the inertia based on density of the shapes and to scale it to the provided mass")
        .value("MASS_AND_INERTIA_PROVIDED", EOverrideMassProperties::MassAndInertiaProvided,
            "Tells the system to take the mass and inertia from mMassPropertiesOverride");

    nb::class_<BodyCreationSettings>(m, "BodyCreationSettings", nb::is_weak_referenceable(), "Settings for constructing a rigid body")
        .def(nb::init<>(), "Default Constructor")
        .def(nb::init<const ShapeSettings *, RVec3::ArgType, Quat, EMotionType, uint16>(),
             "shape_settings"_a, "position"_a, "rotation"_a, "motion_type"_a, "object_layer"_a,
             nb::keep_alive<1, 2>()) // Nurse(self), Patient(ShapeSettings)
        .def(nb::init<const Shape *, RVec3::ArgType, Quat, EMotionType, uint16>(),
            "shape"_a, "position"_a, "rotation"_a, "motion_type"_a, "object_layer"_a,
             nb::keep_alive<1, 2>()) // Nurse(self), Patient(shape)

        .def("set_shape_allowed_dof", [](BodyCreationSettings &self, int in) { // TODO: this is to pass the test
            self.mAllowedDOFs = (EAllowedDOFs)in;
        }, "allowed_dof"_a)

        .def("get_shape_settings", &BodyCreationSettings::GetShapeSettings, nb::rv_policy::reference_internal,
            "Access to the shape settings object. This contains serializable (non-runtime optimized) information about the Shape.")
        .def("set_shape_settings", &BodyCreationSettings::SetShapeSettings, "shape_settings"_a)

        .def("convert_shape_settings", &BodyCreationSettings::ConvertShapeSettings, nb::rv_policy::reference_internal,
            "Convert ShapeSettings object into a Shape object. This will free the ShapeSettings object and make the object ready for runtime. Serialization is no longer possible after this.")

        .def("get_shape", &BodyCreationSettings::GetShape, nb::rv_policy::reference_internal,
            "Access to the run-time shape object. Will convert from ShapeSettings object if needed.")
        .def("set_shape", &BodyCreationSettings::SetShape, "shape"_a)

        .def("has_mass_properties", &BodyCreationSettings::HasMassProperties,
            "Check if the mass properties of this body will be calculated (only relevant for kinematic or dynamic objects that need a MotionProperties object)")
        .def("get_mass_properties", &BodyCreationSettings::GetMassProperties,
            "Calculate (or return when overridden) the mass and inertia for this body")

        .def("save_binary_state", &BodyCreationSettings::SaveBinaryState, "stream"_a,
             "Saves the state of this object in binary form to inStream. Doesn't store the shape nor the group filter.")

        .def("restore_binary_state", &BodyCreationSettings::RestoreBinaryState, "stream"_a,
             "Restore the state of this object from inStream. Doesn't restore the shape nor the group filter.")

        .def("save_with_children", &BodyCreationSettings::SaveWithChildren,
            "stream"_a, "shape_map"_a, "material_map"_a, "group_filter_map"_a,
            "Save body creation settings, its shape, materials and group filter. Pass in an empty map in ioShapeMap / ioMaterialMap / ioGroupFilterMap or reuse the same map while saving multiple shapes to the same stream in order to avoid writing duplicates.\n"
            "Pass nullptr to ioShapeMap and ioMaterial map to skip saving shapes\n"
            "Pass nullptr to ioGroupFilterMap to skip saving group filters")

        .def_static("restore_with_children", &BodyCreationSettings::sRestoreWithChildren,
            "stream"_a, "shape_map"_a, "material_map"_a, "group_filter_map"_a,
            "Restore body creation settings, its shape, materials and group filter. Pass in an empty map in ioShapeMap / ioMaterialMap / ioGroupFilterMap or reuse the same map while reading multiple shapes from the same stream in order to restore duplicates.")

        .def_rw("position", &BodyCreationSettings::mPosition,
            "Position of the body (not of the center of mass)")
        .def_rw("rotation", &BodyCreationSettings::mRotation,
            "Rotation of the body")
        .def_rw("linear_velocity", &BodyCreationSettings::mLinearVelocity,
            "World space linear velocity of the center of mass (m/s)")
        .def_rw("angular_velocity", &BodyCreationSettings::mAngularVelocity,
            "World space angular velocity (rad/s)")

        .def_rw("user_data", &BodyCreationSettings::mUserData, "User data value (can be used by application)")

        .def_rw("object_layer", &BodyCreationSettings::mObjectLayer,
            "The collision layer this body belongs to (determines if two objects can collide)")
        .def_rw("collision_g_group", &BodyCreationSettings::mCollisionGroup,
            "The collision group this body belongs to (determines if two objects can collide)")

        .def_rw("motion_type", &BodyCreationSettings::mMotionType,
            "@name Simulation properties\n"
            "Motion type, determines if the object is static, dynamic or kinematic")
        .def_rw("allowed_dofs", &BodyCreationSettings::mAllowedDOFs,
            "Which degrees of freedom this body has (can be used to limit simulation to 2D)")
        .def_rw("allow_dynamic_or_kinematic", &BodyCreationSettings::mAllowDynamicOrKinematic,
            "When this body is created as static, this setting tells the system to create a MotionProperties object so that the object can be switched to kinematic or dynamic")
        .def_rw("issensor", &BodyCreationSettings::mIsSensor,
            "If this body is a sensor. A sensor will receive collision callbacks, but will not cause any collision responses and can be used as a trigger volume. See description at Body::SetIsSensor.")
        .def_rw("collide_kinematic_vs_non_dynamic", &BodyCreationSettings::mCollideKinematicVsNonDynamic,
            "If kinematic objects can generate contact points against other kinematic or static objects. See description at Body::SetCollideKinematicVsNonDynamic.")
        .def_rw("usemanifold_reduction", &BodyCreationSettings::mUseManifoldReduction,
            "If this body should use manifold reduction (see description at Body::SetUseManifoldReduction)")
        .def_rw("apply_gyroscopic_force", &BodyCreationSettings::mApplyGyroscopicForce,
            "Set to indicate that the gyroscopic force should be applied to this body (aka Dzhanibekov effect, see https://en.wikipedia.org/wiki/Tennis_racket_theorem)")
        .def_rw("motion_quality", &BodyCreationSettings::mMotionQuality,
            "Motion quality, or how well it detects collisions when it has a high velocity")
        .def_rw("enhanced_internal_edge_removal", &BodyCreationSettings::mEnhancedInternalEdgeRemoval,
            "Set to indicate that extra effort should be made to try to remove ghost contacts (collisions with internal edges of a mesh). This is more expensive but makes bodies move smoother over a mesh with convex edges.")
        .def_rw("allow_sleeping", &BodyCreationSettings::mAllowSleeping,
            "If this body can go to sleep or not")
        .def_rw("friction", &BodyCreationSettings::mFriction,
            "Friction of the body (dimensionless number, usually between 0 and 1, 0 = no friction, 1 = friction force equals force that presses the two bodies together). Note that bodies can have negative friction but the combined friction (see PhysicsSystem::SetCombineFriction) should never go below zero.")
        .def_rw("restitution", &BodyCreationSettings::mRestitution,
            "Restitution of body (dimensionless number, usually between 0 and 1, 0 = completely inelastic collision response, 1 = completely elastic collision response). Note that bodies can have negative restitution but the combined restitution (see PhysicsSystem::SetCombineRestitution) should never go below zero.")
        .def_rw("linear_damping", &BodyCreationSettings::mLinearDamping,
            "Linear damping: dv/dt = -c * v. c must be between 0 and 1 but is usually close to 0.")
        .def_rw("angular_damping", &BodyCreationSettings::mAngularDamping,
            "Angular damping: dw/dt = -c * w. c must be between 0 and 1 but is usually close to 0.")
        .def_rw("max_linear_velocity", &BodyCreationSettings::mMaxLinearVelocity,
            "Maximum linear velocity that this body can reach (m/s)")
        .def_rw("max_angular_velocity", &BodyCreationSettings::mMaxAngularVelocity,
            "Maximum angular velocity that this body can reach (rad/s)")
        .def_rw("gravity_factor", &BodyCreationSettings::mGravityFactor,
            "Value to multiply gravity with for this body")
        .def_rw("num_velocity_steps_override", &BodyCreationSettings::mNumVelocityStepsOverride,
            "Used only when this body is dynamic and colliding. Override for the number of solver velocity iterations to run, 0 means use the default in PhysicsSettings::mNumVelocitySteps. The number of iterations to use is the max of all contacts and constraints in the island.")
        .def_rw("num_position_steps_override", &BodyCreationSettings::mNumPositionStepsOverride,
            "Used only when this body is dynamic and colliding. Override for the number of solver position iterations to run, 0 means use the default in PhysicsSettings::mNumPositionSteps. The number of iterations to use is the max of all contacts and constraints in the island.")

        .def_rw("override_mass_properties", &BodyCreationSettings::mOverrideMassProperties,
            "@name Mass properties of the body (by default calculated by the shape)\n"
            "Determines how mMassPropertiesOverride will be used")
        .def_rw("inertia_multiplier", &BodyCreationSettings::mInertiaMultiplier,
            "When calculating the inertia (not when it is provided) the calculated inertia will be multiplied by this value")
        .def_rw("mass_properties_override", &BodyCreationSettings::mMassPropertiesOverride,
            "Contains replacement mass settings which override the automatically calculated values");
}
