#include "Common.h"
#include <Jolt/Physics/Character/CharacterBase.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/Physics/PhysicsSystem.h>

void BindCharacterBase(nb::module_ &m) {
    nb::class_<CharacterBaseSettings, RefTarget<CharacterBaseSettings>> characterBaseSettingsCls(m, "CharacterBaseSettings",
        nb::intrusive_ptr<CharacterBaseSettings>([](CharacterBaseSettings *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Base class for configuration of a character");
    characterBaseSettingsCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const CharacterBaseSettings &>(), "settings"_a)
        .def_rw("up", &CharacterBaseSettings::mUp,
            "Vector indicating the up direction of the character")
        .def_rw("supporting_volume", &CharacterBaseSettings::mSupportingVolume,
            "Plane, defined in local space relative to the character. Every contact behind this plane can support the\n"
            "character, every contact in front of this plane is treated as only colliding with the player.\n"
            "Default: Accept any contact.")
        .def_rw("max_slope_angle", &CharacterBaseSettings::mMaxSlopeAngle,
            "Maximum angle of slope that character can still walk on (radians).")
        .def_rw("enhanced_internal_edge_removal", &CharacterBaseSettings::mEnhancedInternalEdgeRemoval,
            "Set to indicate that extra effort should be made to try to remove ghost contacts (collisions with internal edges of a mesh). This is more expensive but makes bodies move smoother over a mesh with convex edges.")
        .def_rw("shape", &CharacterBaseSettings::mShape,
            "Initial shape that represents the character's volume.\n"
            "Usually this is a capsule, make sure the shape is made so that the bottom of the shape is at (0, 0, 0).")
        .def("__repr__", [](const CharacterBaseSettings &self) {
            std::string shape_repr = (self.mShape != nullptr) ? "<Shape>" : "None";
            return nb::str("CharacterBaseSettings(up={}, max_slope={}, shape={})")
            .format(self.mUp, self.mMaxSlopeAngle, shape_repr);
        });

    nb::class_<CharacterBase, RefTarget<CharacterBase>> characterBaseCls(m, "CharacterBase",
        nb::intrusive_ptr<CharacterBase>([](CharacterBase *o, PyObject *po) noexcept {
            o->set_self_py(po);
    }),"Base class for character class");
    characterBaseCls
        .def(nb::init<const CharacterBaseSettings *, PhysicsSystem *>(), "settings"_a, "system"_a,
            "Constructor")
        .def("set_max_slope_angle", &CharacterBase::SetMaxSlopeAngle, "max_slope_angle"_a,
            "Set the maximum angle of slope that character can still walk on (radians)")
        .def("get_cos_max_slope_angle", &CharacterBase::GetCosMaxSlopeAngle)
        .def("set_up", &CharacterBase::SetUp, "up"_a,
            "Set the up vector for the character")
        .def("get_up", &CharacterBase::GetUp)
        .def("is_slope_too_steep", &CharacterBase::IsSlopeTooSteep, "normal"_a,
            "Check if the normal of the ground surface is too steep to walk on")
        .def("get_shape", &CharacterBase::GetShape, nb::rv_policy::reference,
            "Get the current shape that the character is using.")
        .def_static("to_string", &CharacterBase::sToString, "state"_a, nb::rv_policy::reference,
            "Debug function to convert enum values to string")
        .def("get_ground_state", &CharacterBase::GetGroundState,
            "Current ground state")
        .def("is_supported", &CharacterBase::IsSupported,
            "Returns true if the player is supported by normal or steep ground")
        .def("get_ground_position", &CharacterBase::GetGroundPosition,
            "Get the contact point with the ground")
        .def("get_ground_normal", &CharacterBase::GetGroundNormal,
            "Get the contact normal with the ground")
        .def("get_ground_velocity", &CharacterBase::GetGroundVelocity,
            "Velocity in world space of ground")
        .def("get_ground_material", &CharacterBase::GetGroundMaterial, nb::rv_policy::reference,
            "Material that the character is standing on")
        .def("get_ground_body_id", &CharacterBase::GetGroundBodyID,
            "BodyID of the object the character is standing on. Note may have been removed!")
        .def("get_ground_sub_shape_id", &CharacterBase::GetGroundSubShapeID,
            "Sub part of the body that we're standing on.")
        .def("get_ground_user_data", &CharacterBase::GetGroundUserData,
            "User data value of the body that we're standing on")
        .def("save_state", &CharacterBase::SaveState, "stream"_a)
        .def("restore_state", &CharacterBase::RestoreState, "stream"_a)
        .def("__repr__", [](const CharacterBase &self) {
            return nb::str("<jolt.CharacterBase ground_state={}>").format(CharacterBase::sToString(self.GetGroundState()));
        })
        ;

    nb::enum_<CharacterBase::EGroundState>(characterBaseCls, "EGroundState")
        .value("ON_GROUND", CharacterBase::EGroundState::OnGround,
            "Character is on the ground and can move freely.")
        .value("ON_STEEP_GROUND", CharacterBase::EGroundState::OnSteepGround,
            "Character is on a slope that is too steep and can't climb up any further. The caller should start applying downward velocity if sliding from the slope is desired.")
        .value("NOT_SUPPORTED", CharacterBase::EGroundState::NotSupported,
            "Character is touching an object, but is not supported by it and should fall. The GetGroundXXX functions will return information about the touched object.")
        .value("IN_AIR", CharacterBase::EGroundState::InAir,
            "Character is in the air and is not touching anything.");
}
