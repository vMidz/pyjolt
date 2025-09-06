#include "Common.h"
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>

void BindSoftBodyCreationSettings(nb::module_ &m) {
    nb::class_<SoftBodyCreationSettings> softBodyCreationSettingsCls(m, "SoftBodyCreationSettings",
        "This class contains the information needed to create a soft body object\n"
        "Note: Soft bodies are still in development and come with several caveats. Read the Architecture and API documentation for more information!");
    softBodyCreationSettingsCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const SoftBodySharedSettings *, RVec3Arg, QuatArg, ObjectLayer>(), "settings"_a, "position"_a, "rotation"_a, "object_layer"_a)
        .def("save_binary_state", &SoftBodyCreationSettings::SaveBinaryState, "stream"_a,
            "Saves the state of this object in binary form to inStream. Doesn't store the shared settings nor the group filter.")
        .def("restore_binary_state", &SoftBodyCreationSettings::RestoreBinaryState, "stream"_a,
            "Restore the state of this object from inStream. Doesn't restore the shared settings nor the group filter.")
        .def("save_with_children", &SoftBodyCreationSettings::SaveWithChildren, "stream"_a, "shared_settings_map"_a, "material_map"_a, "group_filter_map"_a,
            "Save this body creation settings, its shared settings and group filter. Pass in an empty map in ioSharedSettingsMap / ioMaterialMap / ioGroupFilterMap or reuse the same map while saving multiple shapes to the same stream in order to avoid writing duplicates.\n"
            "Pass nullptr to ioSharedSettingsMap and ioMaterial map to skip saving shared settings and materials\n"
            "Pass nullptr to ioGroupFilterMap to skip saving group filters")
        .def_static("restore_with_children", &SoftBodyCreationSettings::sRestoreWithChildren, "stream"_a, "shared_settings_map"_a, "material_map"_a, "group_filter_map"_a,
            "Restore a shape, all its children and materials. Pass in an empty map in ioSharedSettingsMap / ioMaterialMap / ioGroupFilterMap or reuse the same map while reading multiple shapes from the same stream in order to restore duplicates.")
        .def_rw("settings", &SoftBodyCreationSettings::mSettings,
            "Defines the configuration of this soft body")
        .def_rw("position", &SoftBodyCreationSettings::mPosition,
            "Initial position of the soft body")
        .def_rw("rotation", &SoftBodyCreationSettings::mRotation,
            "Initial rotation of the soft body")
        .def_rw("user_data", &SoftBodyCreationSettings::mUserData,
            "User data value (can be used by application)")
        .def_rw("object_layer", &SoftBodyCreationSettings::mObjectLayer,
            "@name Collision settings")
        .def_rw("collision_group", &SoftBodyCreationSettings::mCollisionGroup,
            "The collision layer this body belongs to (determines if two objects can collide)")
        .def_rw("num_iterations", &SoftBodyCreationSettings::mNumIterations,
            "Number of solver iterations")
        .def_rw("linear_damping", &SoftBodyCreationSettings::mLinearDamping,
            "Linear damping: dv/dt = -mLinearDamping * v")
        .def_rw("max_linear_velocity", &SoftBodyCreationSettings::mMaxLinearVelocity,
            "Maximum linear velocity that a vertex can reach (m/s)")
        .def_rw("restitution", &SoftBodyCreationSettings::mRestitution,
            "Restitution when colliding")
        .def_rw("friction", &SoftBodyCreationSettings::mFriction,
            "Friction coefficient when colliding")
        .def_rw("pressure", &SoftBodyCreationSettings::mPressure,
            "n * R * T, amount of substance * ideal gas constant * absolute temperature, see https://en.wikipedia.org/wiki/Pressure")
        .def_rw("gravity_factor", &SoftBodyCreationSettings::mGravityFactor,
            "Value to multiply gravity with for this body")
        .def_rw("update_position", &SoftBodyCreationSettings::mUpdatePosition,
            "Update the position of the body while simulating (set to false for something that is attached to the static world)")
        .def_rw("make_rotation_identity", &SoftBodyCreationSettings::mMakeRotationIdentity,
            "Bake specified mRotation in the vertices and set the body rotation to identity (simulation is slightly more accurate if the rotation of a soft body is kept to identity)")
        .def_rw("allow_sleeping", &SoftBodyCreationSettings::mAllowSleeping,
            "If this body can go to sleep or not");
}