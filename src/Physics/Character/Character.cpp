#include "Common.h"
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/ObjectStream/TypeDeclarations.h>

void BindCharacter(nb::module_ &m) {
    nb::class_<CharacterSettings, CharacterBaseSettings> characterSettingsCls(m, "CharacterSettings",
        "Contains the configuration of a character");
    characterSettingsCls
        .def_rw("layer", &CharacterSettings::mLayer,
            "Layer that this character will be added to")
        .def_rw("mass", &CharacterSettings::mMass,
            "Mass of the character")
        .def_rw("friction", &CharacterSettings::mFriction,
            "Friction for the character")
        .def_rw("gravity_factor", &CharacterSettings::mGravityFactor,
            "Value to multiply gravity with for this character")
        .def("__repr__", [](const CharacterSettings &self) {
                return nb::str("CharacterSettings(layer={}, mass={}, friction={}, gravity_factor={})")
                .format(self.mLayer, self.mMass, self.mFriction, self.mGravityFactor);
        });

    nb::class_<Character, CharacterBase> characterCls(m, "Character",
        "Runtime character object.\n"
        "This object usually represents the player or a humanoid AI. It uses a single rigid body,\n"
        "usually with a capsule shape to simulate movement and collision for the character.\n"
        "The character is a keyframed object, the application controls it by setting the velocity.");
    characterCls
        .def(nb::init<const CharacterSettings *, RVec3Arg, QuatArg, uint64, PhysicsSystem *>(),
            "settings"_a, "position"_a, "rotation"_a, "user_data"_a, "system"_a,
            "Constructor.\n"
            "Args:\n"
            "    settings (CharacterSettings*): The settings for the character.\n"
            "    position (Vec3): Initial position for the character.\n"
            "    rotation (Quat): Initial rotation for the character (usually only around Y).\n"
            "    user_data (int): Application specific value.\n"
            "    system (PhysicsSystem*): Physics system that this character will be added to later.")
        .def("add_to_physics_system", &Character::AddToPhysicsSystem, "activation_mode"_a = EActivation::Activate, "lock_bodies"_a = true,
            "Add bodies and constraints to the system and optionally activate the bodies")
        .def("remove_from_physics_system", &Character::RemoveFromPhysicsSystem, "lock_bodies"_a = true,
            "Remove bodies and constraints from the system")
        .def("activate", &Character::Activate, "lock_bodies"_a = true,
            "Wake up the character")
        .def("post_simulation", &Character::PostSimulation, "max_separation_distance"_a, "lock_bodies"_a = true,
            "Needs to be called after every PhysicsSystem::Update.\n"
            "Args:\n"
            "    max_separation_distance (float): Max distance between the floor and the character to still consider the character standing on the floor.\n"
            "    lock_bodies (bool): If the collision query should use the locking body interface (true) or the non locking body interface (false).")
        .def("set_linear_and_angular_velocity", &Character::SetLinearAndAngularVelocity, "linear_velocity"_a, "angular_velocity"_a, "lock_bodies"_a = true,
            "Control the velocity of the character")
        .def("get_linear_velocity", &Character::GetLinearVelocity, "lock_bodies"_a = true,
            "Get the linear velocity of the character (m / s)")
        .def("set_linear_velocity", &Character::SetLinearVelocity, "linear_velocity"_a, "lock_bodies"_a = true,
            "Set the linear velocity of the character (m / s)")
        .def("add_linear_velocity", &Character::AddLinearVelocity, "linear_velocity"_a, "lock_bodies"_a = true,
            "Add world space linear velocity to current velocity (m / s)")
        .def("add_impulse", &Character::AddImpulse, "impulse"_a, "lock_bodies"_a = true,
            "Add impulse to the center of mass of the character")
        .def("get_body_id", &Character::GetBodyID,
            "Get the body associated with this character")
        .def("get_position_and_rotation", &Character::GetPositionAndRotation, "position"_a, "rotation"_a, "lock_bodies"_a = true,
            "Get position / rotation of the body")
        .def("set_position_and_rotation", &Character::SetPositionAndRotation, "position"_a, "rotation"_a, "activation_mode"_a = EActivation::Activate, "lock_bodies"_a = true,
            "Set the position / rotation of the body, optionally activating it.")
        .def("get_position", &Character::GetPosition, "lock_bodies"_a = true,
            "Get the position of the character")
        .def("set_position", &Character::SetPosition, "postion"_a, "activation_mode"_a = EActivation::Activate, "lock_bodies"_a = true,
            "Set the position of the character, optionally activating it.")
        .def("get_rotation", &Character::GetRotation, "lock_bodies"_a = true,
            "Get the rotation of the character")
        .def("set_rotation", &Character::SetRotation, "rotation"_a, "activation_mode"_a = EActivation::Activate, "lock_bodies"_a = true,
            "Set the rotation of the character, optionally activating it.")
        .def("get_center_of_mass_position", &Character::GetCenterOfMassPosition, "lock_bodies"_a = true,
            "Position of the center of mass of the underlying rigid body")
        .def("get_world_transform", &Character::GetWorldTransform, "lock_bodies"_a = true,
            "Calculate the world transform of the character")
        .def("get_layer", &Character::GetLayer,
            "Get the layer of the character")
        .def("set_layer", &Character::SetLayer, "layer"_a, "lock_bodies"_a = true,
            "Update the layer of the character")
        .def("set_shape", &Character::SetShape, "shape"_a, "max_penetration_depth"_a, "lock_bodies"_a = true,
            "Switch the shape of the character (e.g. for stance). When inMaxPenetrationDepth is not FLT_MAX, it checks\n"
            "if the new shape collides before switching shape. Returns true if the switch succeeded.")
        .def("get_transformed_shape", &Character::GetTransformedShape, "lock_bodies"_a = true,
            "Get the transformed shape that represents the volume of the character, can be used for collision checks.")
        .def("check_collision", nb::overload_cast<RVec3Arg, QuatArg, Vec3Arg,
            float, const Shape *, RVec3Arg, CollideShapeCollector &, bool>(&Character::CheckCollision, nb::const_),
            "position"_a, "rotation"_a, "movement_direction"_a, "max_separation_distance"_a, "shape"_a, "base_offset"_a, "collector"_a, "lock_bodies"_a = true,
            "@brief Get all contacts for the character at a particular location.\n"
            "Args:\n"
            "    position (Vec3): Position to test.\n"
            "    rotation (Quat): Rotation at which to test the shape.\n"
            "    movement_direction (Vec3): A hint in which direction the character is moving, will be used to calculate a proper normal.\n"
            "    max_separation_distance (float): How much distance around the character you want to report contacts in (can be 0 to match the character exactly).\n"
            "    shape (Shape*): Shape to test collision with.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. GetPosition() since floats are most accurate near the origin.\n"
            "    collector (CollideShapeCollector): Collision collector that receives the collision results.\n"
            "    lock_bodies (bool): If the collision query should use the locking body interface (true) or the non locking body interface (false).")
        .def("__repr__", [](const Character &self) {
            return nb::str("<jolt.Character body_id={}>").format(self.GetBodyID());
        });

}