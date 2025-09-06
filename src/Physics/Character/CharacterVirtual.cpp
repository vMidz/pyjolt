#include "Common.h"
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/InternalEdgeRemovingCollector.h>
#include <Jolt/Core/QuickSort.h>
#include <Jolt/Geometry/ConvexSupport.h>
#include <Jolt/Geometry/GJKClosestPoint.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <nanobind/trampoline.h>

class PyCharacterContactListener : public CharacterContactListener {
  public:
    NB_TRAMPOLINE(CharacterContactListener, 7);

    void OnAdjustBodyVelocity(const CharacterVirtual *inCharacter,
                            const Body &inBody2,
                            Vec3 &ioLinearVelocity,
                            Vec3 &ioAngularVelocity) override {

        NB_OVERRIDE(OnAdjustBodyVelocity,
        inCharacter,
        inBody2,
        ioLinearVelocity,
        ioAngularVelocity);
    }

    bool OnContactValidate(const CharacterVirtual *inCharacter,
                            const BodyID &inBodyID2,
                            const SubShapeID &inSubShapeID2) override {
        NB_OVERRIDE(OnContactValidate, inCharacter, inBodyID2, inSubShapeID2);
    }

    bool OnCharacterContactValidate(const CharacterVirtual *inCharacter,
                                    const CharacterVirtual *inOtherCharacter,
                                    const SubShapeID &inSubShapeID2) override {

        NB_OVERRIDE(OnCharacterContactValidate, inCharacter, inOtherCharacter, inSubShapeID2);
    }
    void OnContactAdded(const CharacterVirtual *inCharacter,
                        const BodyID &inBodyID2,
                        const SubShapeID &inSubShapeID2,
                        RVec3Arg inContactPosition,
                        Vec3Arg inContactNormal,
                        CharacterContactSettings &ioSettings) override {

        NB_OVERRIDE(OnContactAdded,
                    inCharacter,
                    inBodyID2,
                    inSubShapeID2,
                    inContactPosition,
                    inContactNormal,
                    ioSettings);
    }
    void OnCharacterContactAdded(const CharacterVirtual *inCharacter, const CharacterVirtual *inOtherCharacter,
            const SubShapeID &inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings &ioSettings) override {

        NB_OVERRIDE(OnCharacterContactAdded, inCharacter, inOtherCharacter,
            inSubShapeID2, inContactPosition, inContactNormal, ioSettings);
    }
    void OnContactSolve(const CharacterVirtual *inCharacter, const BodyID &inBodyID2, const SubShapeID &inSubShapeID2,
        RVec3Arg inContactPosition, Vec3Arg inContactNormal, Vec3Arg inContactVelocity,
            const PhysicsMaterial *inContactMaterial, Vec3Arg inCharacterVelocity, Vec3 &ioNewCharacterVelocity) override {

        NB_OVERRIDE(OnContactSolve, inCharacter, inBodyID2, inSubShapeID2, inContactPosition,
            inContactNormal, inContactVelocity, inContactMaterial, inCharacterVelocity, ioNewCharacterVelocity);
    }
    void OnCharacterContactSolve(const CharacterVirtual *inCharacter, const CharacterVirtual *inOtherCharacter,
        const SubShapeID &inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, Vec3Arg inContactVelocity,
            const PhysicsMaterial *inContactMaterial, Vec3Arg inCharacterVelocity, Vec3 &ioNewCharacterVelocity) override {
        NB_OVERRIDE(OnCharacterContactSolve, inCharacter, inOtherCharacter, inSubShapeID2, inContactPosition,
            inContactNormal, inContactVelocity, inContactMaterial, inCharacterVelocity, ioNewCharacterVelocity);
    }
};

class PyCharacterVsCharacterCollision : public CharacterVsCharacterCollision {
  public:
    NB_TRAMPOLINE(CharacterVsCharacterCollision, 2);

    void CollideCharacter(const CharacterVirtual *inCharacter, RMat44Arg inCenterOfMassTransform,
        const CollideShapeSettings &inCollideShapeSettings, RVec3Arg inBaseOffset, CollideShapeCollector &ioCollector) const override {
        NB_OVERRIDE_PURE_NAME(
            "collide_character",
            CollideCharacter,
            inCharacter,
            inCenterOfMassTransform,
            inCollideShapeSettings,
            inBaseOffset,
            ioCollector);
    }
    void CastCharacter(const CharacterVirtual *inCharacter, RMat44Arg inCenterOfMassTransform,
        Vec3Arg inDirection, const ShapeCastSettings &inShapeCastSettings, RVec3Arg inBaseOffset, CastShapeCollector &ioCollector) const override {
        NB_OVERRIDE_PURE_NAME("cast_character",
                              CastCharacter,
                              inCharacter,
                              inCenterOfMassTransform,
                              inDirection,
                              inShapeCastSettings,
                              inBaseOffset,
                              ioCollector);
    }
};

void BindCharacterVirtual(nb::module_ &m) {
    nb::class_<CharacterVirtual, CharacterBase> characterVirtualCls(m, "CharacterVirtual",
        "Runtime character object.\n"
        "This object usually represents the player. Contrary to the Character class it doesn't use a rigid body but moves doing collision checks only (hence the name virtual).\n"
        "The advantage of this is that you can determine when the character moves in the frame (usually this has to happen at a very particular point in the frame)\n"
        "but the downside is that other objects don't see this virtual character. In order to make this work it is recommended to pair a CharacterVirtual with a Character that\n"
        "moves along. This Character should be keyframed (or at least have no gravity) and move along with the CharacterVirtual so that other rigid bodies can collide with it.");
    characterVirtualCls
        .def(nb::init<const CharacterVirtualSettings *, RVec3Arg, QuatArg, uint64, PhysicsSystem *>(), "settings"_a, "position"_a, "rotation"_a, "user_data"_a, "system"_a,
            "Constructor.\n"
            "Args:\n"
            "    settings (CharacterVirtualSettings*): The settings for the character.\n"
            "    position (Vec3): Initial position for the character.\n"
            "    rotation (Quat): Initial rotation for the character (usually only around the up-axis).\n"
            "    user_data (int): Application specific value.\n"
            "    system (PhysicsSystem*): Physics system that this character will be added to.")
        .def(nb::init<const CharacterVirtualSettings *, RVec3Arg, QuatArg, PhysicsSystem *>(), "settings"_a, "position"_a, "rotation"_a, "system"_a,
            "Constructor without user data")
        .def("set_listener", &CharacterVirtual::SetListener, "listener"_a,
            "Set the contact listener")
        .def("get_listener", &CharacterVirtual::GetListener, nb::rv_policy::reference,
            "Get the current contact listener")
        .def("set_character_vs_character_collision", &CharacterVirtual::SetCharacterVsCharacterCollision, "character_vs_character_collision"_a,
            "Set the character vs character collision interface")
        .def("get_linear_velocity", &CharacterVirtual::GetLinearVelocity,
            "Get the linear velocity of the character (m / s)")
        .def("set_linear_velocity", &CharacterVirtual::SetLinearVelocity, "linear_velocity"_a,
            "Set the linear velocity of the character (m / s)")
        .def("get_position", &CharacterVirtual::GetPosition,
            "Get the position of the character")
        .def("set_position", &CharacterVirtual::SetPosition, "position"_a,
            "Set the position of the character")
        .def("get_rotation", &CharacterVirtual::GetRotation,
            "Get the rotation of the character")
        .def("set_rotation", &CharacterVirtual::SetRotation, "rotation"_a,
            "Set the rotation of the character")
        .def("get_center_of_mass_position", &CharacterVirtual::GetCenterOfMassPosition)
        .def("get_world_transform", &CharacterVirtual::GetWorldTransform,
            "Calculate the world transform of the character")
        .def("get_center_of_mass_transform", nb::overload_cast<>(&CharacterVirtual::GetCenterOfMassTransform, nb::const_),
            "Calculates the transform for this character's center of mass")
        .def("get_mass", &CharacterVirtual::GetMass,
            "Character mass (kg)")
        .def("set_mass", &CharacterVirtual::SetMass, "mass"_a)
        .def("get_max_strength", &CharacterVirtual::GetMaxStrength,
            "Maximum force with which the character can push other bodies (N)")
        .def("set_max_strength", &CharacterVirtual::SetMaxStrength, "max_strength"_a)
        .def("get_penetration_recovery_speed", &CharacterVirtual::GetPenetrationRecoverySpeed,
            "This value governs how fast a penetration will be resolved, 0 = nothing is resolved, 1 = everything in one update")
        .def("set_penetration_recovery_speed", &CharacterVirtual::SetPenetrationRecoverySpeed, "speed"_a)
        .def("get_enhanced_internal_edge_removal", &CharacterVirtual::GetEnhancedInternalEdgeRemoval,
            "Set to indicate that extra effort should be made to try to remove ghost contacts (collisions with internal edges of a mesh). This is more expensive but makes bodies move smoother over a mesh with convex edges.")
        .def("set_enhanced_internal_edge_removal", &CharacterVirtual::SetEnhancedInternalEdgeRemoval, "apply"_a)
        .def("get_character_padding", &CharacterVirtual::GetCharacterPadding,
            "Character padding")
        .def("get_max_num_hits", &CharacterVirtual::GetMaxNumHits,
            "Max num hits to collect in order to avoid excess of contact points collection")
        .def("set_max_num_hits", &CharacterVirtual::SetMaxNumHits, "max_hits"_a)
        .def("get_hit_reduction_cos_max_angle", &CharacterVirtual::GetHitReductionCosMaxAngle,
            "Cos(angle) where angle is the maximum angle between two hits contact normals that are allowed to be merged during hit reduction. Default is around 2.5 degrees. Set to -1 to turn off.")
        .def("set_hit_reduction_cos_max_angle", &CharacterVirtual::SetHitReductionCosMaxAngle, "cos_max_angle"_a)
        .def("get_max_hits_exceeded", &CharacterVirtual::GetMaxHitsExceeded,
            "Returns if we exceeded the maximum number of hits during the last collision check and had to discard hits based on distance.\n"
            "This can be used to find areas that have too complex geometry for the character to navigate properly.\n"
            "To solve you can either increase the max number of hits or simplify the geometry. Note that the character simulation will\n"
            "try to do its best to select the most relevant contacts to avoid the character from getting stuck.")
        .def("get_shape_offset", &CharacterVirtual::GetShapeOffset,
            "An extra offset applied to the shape in local space. This allows applying an extra offset to the shape in local space. Note that setting it on the fly can cause the shape to teleport into collision.")
        .def("set_shape_offset", &CharacterVirtual::SetShapeOffset, "shape_offset"_a)
        .def("get_user_data", &CharacterVirtual::GetUserData,
            "Access to the user data, can be used for anything by the application")
        .def("set_user_data", &CharacterVirtual::SetUserData, "user_data"_a)
        .def("get_inner_body_id", &CharacterVirtual::GetInnerBodyID,
            "Optional inner rigid body that proxies the character in the world. Can be used to update body properties.")
        .def("cancel_velocity_towards_steep_slopes", &CharacterVirtual::CancelVelocityTowardsSteepSlopes, "desired_velocity"_a,
            "This function can be called prior to calling Update() to convert a desired velocity into a velocity that won't make the character move further onto steep slopes.\n"
            "This velocity can then be set on the character using SetLinearVelocity().\n"
            "Args:\n"
            "    desired_velocity (Vec3): Velocity to clamp against steep walls.\n"
            "Returns:\n"
            "    Vec3: A new velocity vector that won't make the character move up steep slopes.")
        .def("update", &CharacterVirtual::Update, "delta_time"_a, "gravity"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a, "allocator"_a,
            "This is the main update function. It moves the character according to its current velocity (the character is similar to a kinematic body in the sense\n"
            "that you set the velocity and the character will follow unless collision is blocking the way). Note it's your own responsibility to apply gravity to the character velocity!\n"
            "Different surface materials (like ice) can be emulated by getting the ground material and adjusting the velocity and/or the max slope angle accordingly every frame.\n"
            "Args:\n"
            "    delta_time (float): Time step to simulate.\n"
            "    gravity (Vec3): Gravity vector (m/s^2). This gravity vector is only used when the character is standing on top of another object to apply downward force.\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that is used to check if the character collides with something in the broadphase.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that is used to check if a character collides with a layer.\n"
            "    body_filter (BodyFilter): Filter that is used to check if a character collides with a body.\n"
            "    shape_filter (ShapeFilter): Filter that is used to check if a character collides with a subshape.\n"
            "    allocator (TempAllocator): An allocator for temporary allocations. All memory will be freed by the time this function returns.")
        .def("can_walk_stairs", &CharacterVirtual::CanWalkStairs, "linear_velocity"_a,
            "This function will return true if the character has moved into a slope that is too steep (e.g. a vertical wall).\n"
            "You would call WalkStairs to attempt to step up stairs.\n"
            "Args:\n"
            "    linear_velocity (Vec3): The linear velocity that the player desired. This is used to determine if we're pushing into a step.")
        .def("walk_stairs", &CharacterVirtual::WalkStairs, "delta_time"_a, "step_up"_a, "step_forward"_a, "step_forward_test"_a, "step_down_extra"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a, "allocator"_a,
            "When stair walking is needed, you can call the WalkStairs function to cast up, forward and down again to try to find a valid position.\n"
            "Args:\n"
            "    delta_time (float): Time step to simulate.\n"
            "    step_up (Vec3): The direction and distance to step up (this corresponds to the max step height).\n"
            "    step_forward (Vec3): The direction and distance to step forward after the step up.\n"
            "    step_forward_test (Vec3): When running at a high frequency, inStepForward can be very small and it's likely that you hit the side of the stairs on the way down. This could produce a normal that violates the max slope angle. If this happens, we test again using this distance from the up position to see if we find a valid slope.\n"
            "    step_down_extra (Vec3): An additional translation that is added when stepping down at the end. Allows you to step further down than up. Set to zero if you don't want this. Should be in the opposite direction of up.\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that is used to check if the character collides with something in the broadphase.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that is used to check if a character collides with a layer.\n"
            "    body_filter (BodyFilter): Filter that is used to check if a character collides with a body.\n"
            "    shape_filter (ShapeFilter): Filter that is used to check if a character collides with a subshape.\n"
            "    allocator (TempAllocator): An allocator for temporary allocations. All memory will be freed by the time this function returns.\n"
            "Returns:\n"
            "    bool: true if the stair walk was successful.")
        .def("stick_to_floor", &CharacterVirtual::StickToFloor, "step_down"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a, "allocator"_a,
            "This function can be used to artificially keep the character to the floor. Normally when a character is on a small step and starts moving horizontally, the character will\n"
            "lose contact with the floor because the initial vertical velocity is zero while the horizontal velocity is quite high. To prevent the character from losing contact with the floor,\n"
            "we do an additional collision check downwards and if we find the floor within a certain distance, we project the character onto the floor.\n"
            "Args:\n"
            "    step_down (Vec3): Max amount to project the character downwards (if no floor is found within this distance, the function will return false).\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that is used to check if the character collides with something in the broadphase.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that is used to check if a character collides with a layer.\n"
            "    body_filter (BodyFilter): Filter that is used to check if a character collides with a body.\n"
            "    shape_filter (ShapeFilter): Filter that is used to check if a character collides with a subshape.\n"
            "    allocator (TempAllocator): An allocator for temporary allocations. All memory will be freed by the time this function returns.\n"
            "Returns:\n"
            "    bool: True if the character was successfully projected onto the floor.")
        .def("extended_update", &CharacterVirtual::ExtendedUpdate, "delta_time"_a, "gravity"_a, "settings"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a, "allocator"_a,
            "This function combines Update, StickToFloor and WalkStairs. This function serves as an example of how these functions could be combined.\n"
            "Before calling, call SetLinearVelocity to update the horizontal/vertical speed of the character, typically this is:\n"
            "- When on OnGround and not moving away from ground: velocity = GetGroundVelocity() + horizontal speed as input by player + optional vertical jump velocity + delta time * gravity\n"
            "- Else: velocity = current vertical velocity + horizontal speed as input by player + delta time * gravity.\n"
            "Args:\n"
            "    delta_time (float): Time step to simulate.\n"
            "    gravity (Vec3): Gravity vector (m/s^2). This gravity vector is only used when the character is standing on top of another object to apply downward force.\n"
            "    settings (ExtendedUpdateSettings): A structure containing settings for the algorithm.\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that is used to check if the character collides with something in the broadphase.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that is used to check if a character collides with a layer.\n"
            "    body_filter (BodyFilter): Filter that is used to check if a character collides with a body.\n"
            "    shape_filter (ShapeFilter): Filter that is used to check if a character collides with a subshape.\n"
            "    allocator (TempAllocator): An allocator for temporary allocations. All memory will be freed by the time this function returns.")
        .def("refresh_contacts", &CharacterVirtual::RefreshContacts, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a, "allocator"_a,
            "This function can be used after a character has teleported to determine the new contacts with the world.")
        .def("update_ground_velocity", &CharacterVirtual::UpdateGroundVelocity,
            "Use the ground body ID to get an updated estimate of the ground velocity. This function can be used if the ground body has moved / changed velocity and you want a new estimate of the ground velocity.\n"
            "It will not perform collision detection, so is less accurate than RefreshContacts but a lot faster.")
        .def("set_shape", &CharacterVirtual::SetShape, "shape"_a, "max_penetration_depth"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a, "allocator"_a,
            "Switch the shape of the character (e.g. for stance).\n"
            "Args:\n"
            "    shape (Shape*): The shape to switch to.\n"
            "    max_penetration_depth (float): When inMaxPenetrationDepth is not FLT_MAX, it checks if the new shape collides before switching shape. This is the max penetration we're willing to accept after the switch.\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that is used to check if the character collides with something in the broadphase.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that is used to check if a character collides with a layer.\n"
            "    body_filter (BodyFilter): Filter that is used to check if a character collides with a body.\n"
            "    shape_filter (ShapeFilter): Filter that is used to check if a character collides with a subshape.\n"
            "    allocator (TempAllocator): An allocator for temporary allocations. All memory will be freed by the time this function returns.\n"
            "Returns:\n"
            "    bool: Returns true if the switch succeeded.")
        .def("set_inner_body_shape", &CharacterVirtual::SetInnerBodyShape, "shape"_a,
            "Updates the shape of the inner rigid body. Should be called after a successful call to SetShape.")
        .def("get_transformed_shape", &CharacterVirtual::GetTransformedShape,
            "Get the transformed shape that represents the volume of the character, can be used for collision checks.")
        .def("check_collision", &CharacterVirtual::CheckCollision, "position"_a, "rotation"_a, "movement_direction"_a, "max_separation_distance"_a, "shape"_a, "base_offset"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a,
            "@brief Get all contacts for the character at a particular location.\n"
            "When colliding with another character virtual, this pointer will be provided through CollideShapeCollector::SetUserContext before adding a hit.\n"
            "Args:\n"
            "    position (Vec3): Position to test, note that this position will be corrected for the character padding.\n"
            "    rotation (Quat): Rotation at which to test the shape.\n"
            "    movement_direction (Vec3): A hint in which direction the character is moving, will be used to calculate a proper normal.\n"
            "    max_separation_distance (float): How much distance around the character you want to report contacts in (can be 0 to match the character exactly).\n"
            "    shape (Shape*): Shape to test collision with.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. GetPosition() since floats are most accurate near the origin.\n"
            "    collector (CollideShapeCollector): Collision collector that receives the collision results.\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that is used to check if the character collides with something in the broadphase.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that is used to check if a character collides with a layer.\n"
            "    body_filter (BodyFilter): Filter that is used to check if a character collides with a body.\n"
            "    shape_filter (ShapeFilter): Filter that is used to check if a character collides with a subshape.")
        .def("save_state", &CharacterVirtual::SaveState, "stream"_a)
        .def("restore_state", &CharacterVirtual::RestoreState, "stream"_a)
        .def("get_active_contacts", &CharacterVirtual::GetActiveContacts,
            "Access to the internal list of contacts that the character has found.")
        .def("has_collided_with", nb::overload_cast<const BodyID &>(&CharacterVirtual::HasCollidedWith, nb::const_), "body"_a,
            "Check if the character is currently in contact with or has collided with another body in the last time step")
        .def("has_collided_with", nb::overload_cast<const CharacterVirtual *>(&CharacterVirtual::HasCollidedWith, nb::const_), "character"_a,
            "Check if the character is currently in contact with or has collided with another character in the last time step")
        .def_rw_static("draw_constraints", &CharacterVirtual::sDrawConstraints,
            "Draw the current state of the constraints for iteration 0 when creating them")
        .def_rw_static("draw_walk_stairs", &CharacterVirtual::sDrawWalkStairs,
            "Draw the state of the walk stairs algorithm")
        .def_rw_static("draw_stick_to_floor", &CharacterVirtual::sDrawStickToFloor,
            "Draw the state of the stick to floor algorithm");

    nb::class_<CharacterVirtualSettings, CharacterBaseSettings> characterVirtualSettingsCls(m, "CharacterVirtualSettings",
        "Contains the configuration of a character");
    characterVirtualSettingsCls
        .def_rw("mass", &CharacterVirtualSettings::mMass,
            "Character mass (kg). Used to push down objects with gravity when the character is standing on top.")
        .def_rw("max_strength", &CharacterVirtualSettings::mMaxStrength,
            "Maximum force with which the character can push other bodies (N).")
        .def_rw("shape_offset", &CharacterVirtualSettings::mShapeOffset,
            "An extra offset applied to the shape in local space. This allows applying an extra offset to the shape in local space.")
        .def_rw("back_face_mode", &CharacterVirtualSettings::mBackFaceMode,
            "When colliding with back faces, the character will not be able to move through back facing triangles. Use this if you have triangles that need to collide on both sides.")
        .def_rw("predictive_contact_distance", &CharacterVirtualSettings::mPredictiveContactDistance,
            "When colliding with back faces, the character will not be able to move through back facing triangles. Use this if you have triangles that need to collide on both sides.")
        .def_rw("max_collision_iterations", &CharacterVirtualSettings::mMaxCollisionIterations,
            "How far to scan outside of the shape for predictive contacts. A value of 0 will most likely cause the character to get stuck as it cannot properly calculate a sliding direction anymore. A value that's too high will cause ghost collisions.")
        .def_rw("max_constraint_iterations", &CharacterVirtualSettings::mMaxConstraintIterations,
            "Max amount of collision loops")
        .def_rw("min_time_remaining", &CharacterVirtualSettings::mMinTimeRemaining,
            "How often to try stepping in the constraint solving")
        .def_rw("collision_tolerance", &CharacterVirtualSettings::mCollisionTolerance,
            "Early out condition: If this much time is left to simulate we are done")
        .def_rw("character_padding", &CharacterVirtualSettings::mCharacterPadding,
            "How far we're willing to penetrate geometry")
        .def_rw("max_num_hits", &CharacterVirtualSettings::mMaxNumHits,
            "How far we try to stay away from the geometry, this ensures that the sweep will hit as little as possible lowering the collision cost and reducing the risk of getting stuck")
        .def_rw("hit_reduction_cos_max_angle", &CharacterVirtualSettings::mHitReductionCosMaxAngle,
            "Max num hits to collect in order to avoid excess of contact points collection")
        .def_rw("penetration_recovery_speed", &CharacterVirtualSettings::mPenetrationRecoverySpeed,
            "Cos(angle) where angle is the maximum angle between two hits contact normals that are allowed to be merged during hit reduction. Default is around 2.5 degrees. Set to -1 to turn off.")
        .def_rw("inner_body_shape", &CharacterVirtualSettings::mInnerBodyShape,
            "This character can optionally have an inner rigid body. This rigid body can be used to give the character presence in the world. When set it means that:\n"
            "- Regular collision checks (e.g. NarrowPhaseQuery::CastRay) will collide with the rigid body (they cannot collide with CharacterVirtual since it is not added to the broad phase)\n"
            "- Regular contact callbacks will be called through the ContactListener (next to the ones that will be passed to the CharacterContactListener)\n"
            "- Fast moving objects of motion quality LinearCast will not be able to pass through the CharacterVirtual in 1 time step")
        .def_rw("inner_body_layer", &CharacterVirtualSettings::mInnerBodyLayer,
            "Layer that the inner rigid body will be added to");

    nb::class_<CharacterContactSettings> characterContactSettingsCls(m, "CharacterContactSettings",
        "This class contains settings that allow you to override the behavior of a character's collision response");
    characterContactSettingsCls
        .def_rw("can_push_character", &CharacterContactSettings::mCanPushCharacter,
            "True when the object can push the virtual character.")
        .def_rw("can_receive_impulses", &CharacterContactSettings::mCanReceiveImpulses,
            "True when the virtual character can apply impulses (push) the body.\n"
            "Note that this only works against rigid bodies. Other CharacterVirtual objects can only be moved in their own update,\n"
            "so you must ensure that in their OnCharacterContactAdded mCanPushCharacter is true.");

    nb::class_<CharacterContactListener> characterContactListenerCls(m, "CharacterContactListener",
        "This class receives callbacks when a virtual character hits something.");
    characterContactListenerCls
        .def("on_adjust_body_velocity", &CharacterContactListener::OnAdjustBodyVelocity, "character"_a, "body2"_a, "linear_velocity"_a, "angular_velocity"_a,
            "Callback to adjust the velocity of a body as seen by the character. Can be adjusted to e.g. implement a conveyor belt or an inertial dampener system of a sci-fi space ship.\n"
            "Note that inBody2 is locked during the callback so you can read its properties freely.")
        .def("on_contact_validate", &CharacterContactListener::OnContactValidate, "character"_a, "body_id2"_a, "sub_shape_id2"_a,
            "Checks if a character can collide with specified body. Return true if the contact is valid.")
        .def("on_character_contact_validate", &CharacterContactListener::OnCharacterContactValidate, "character"_a, "other_character"_a, "sub_shape_id2"_a,
            "Same as OnContactValidate but when colliding with a CharacterVirtual")
        .def("on_contact_added", &CharacterContactListener::OnContactAdded, "character"_a, "body_id2"_a, "sub_shape_id2"_a, "contact_position"_a, "contact_normal"_a, "settings"_a,
            "Called whenever the character collides with a body.\n"
            "Args:\n"
            "    character (CharacterVirtual*): Character that is being solved.\n"
            "    body_id2 (BodyID): Body ID of body that is being hit.\n"
            "    sub_shape_id2 (SubShapeID): Sub shape ID of shape that is being hit.\n"
            "    contact_position (Vec3): World space contact position.\n"
            "    contact_normal (Vec3): World space contact normal.\n"
            "    settings (CharacterContactSettings): Settings returned by the contact callback to indicate how the character should behave.")
        .def("on_character_contact_added", &CharacterContactListener::OnCharacterContactAdded, "character"_a, "other_character"_a, "sub_shape_id2"_a, "contact_position"_a, "contact_normal"_a, "settings"_a,
            "Same as OnContactAdded but when colliding with a CharacterVirtual")
        .def("on_contact_solve", &CharacterContactListener::OnContactSolve, "character"_a, "body_id2"_a, "sub_shape_id2"_a, "contact_position"_a, "contact_normal"_a, "contact_velocity"_a, "contact_material"_a, "character_velocity"_a, "new_character_velocity"_a,
            "Called whenever a contact is being used by the solver. Allows the listener to override the resulting character velocity (e.g. by preventing sliding along certain surfaces).\n"
            "Args:\n"
            "    character (CharacterVirtual*): Character that is being solved.\n"
            "    body_id2 (BodyID): Body ID of body that is being hit.\n"
            "    sub_shape_id2 (SubShapeID): Sub shape ID of shape that is being hit.\n"
            "    contact_position (Vec3): World space contact position.\n"
            "    contact_normal (Vec3): World space contact normal.\n"
            "    contact_velocity (Vec3): World space velocity of contact point (e.g. for a moving platform).\n"
            "    contact_material (PhysicsMaterial*): Material of contact point.\n"
            "    character_velocity (Vec3): World space velocity of the character prior to hitting this contact.\n"
            "    new_character_velocity (Vec3): Contains the calculated world space velocity of the character after hitting this contact, this velocity slides along the surface of the contact. Can be modified by the listener to provide an alternative velocity.")
        .def("on_character_contact_solve", &CharacterContactListener::OnCharacterContactSolve, "character"_a, "other_character"_a, "sub_shape_id2"_a, "contact_position"_a, "contact_normal"_a, "contact_velocity"_a, "contact_material"_a, "character_velocity"_a, "new_character_velocity"_a,
            "Same as OnContactSolve but when colliding with a CharacterVirtual");

    nb::class_<CharacterVsCharacterCollision, NonCopyable> characterVsCharacterCollisionCls(m, "CharacterVsCharacterCollision",
        "Interface class that allows a CharacterVirtual to check collision with other CharacterVirtual instances.\n"
        "Since CharacterVirtual instances are not registered anywhere, it is up to the application to test collision against relevant characters.\n"
        "The characters could be stored in a tree structure to make this more efficient.");
    characterVsCharacterCollisionCls
        .def("collide_character", &CharacterVsCharacterCollision::CollideCharacter, "character"_a, "center_of_mass_transform"_a, "collide_shape_settings"_a, "base_offset"_a, "collector"_a,
            "Collide a character against other CharacterVirtuals.\n"
            "Args:\n"
            "    character (CharacterVirtual*): The character to collide.\n"
            "    center_of_mass_transform (Mat44): Center of mass transform for this character.\n"
            "    collide_shape_settings (CollideShapeSettings): Settings for the collision check.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. GetPosition() since floats are most accurate near the origin.\n"
            "    collector (CollideShapeCollector): Collision collector that receives the collision results.")
        .def("cast_character", &CharacterVsCharacterCollision::CastCharacter, "character"_a, "center_of_mass_transform"_a, "direction"_a, "shape_cast_settings"_a, "base_offset"_a, "collector"_a,
            "Cast a character against other CharacterVirtuals.\n"
            "Args:\n"
            "    character (CharacterVirtual*): The character to cast.\n"
            "    center_of_mass_transform (Mat44): Center of mass transform for this character.\n"
            "    direction (Vec3): Direction and length to cast in.\n"
            "    shape_cast_settings (ShapeCastSettings): Settings for the shape cast.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. GetPosition() since floats are most accurate near the origin.\n"
            "    collector (CastShapeCollector): Collision collector that receives the collision results.");

    nb::class_<CharacterVsCharacterCollisionSimple, CharacterVsCharacterCollision> characterVsCharacterCollisionSimpleCls(m, "CharacterVsCharacterCollisionSimple",
        "Simple collision checker that loops over all registered characters.\n"
        "Note that this is not thread safe, so make sure that only one CharacterVirtual is checking collision at a time.");
    characterVsCharacterCollisionSimpleCls
        .def("add", &CharacterVsCharacterCollisionSimple::Add, "character"_a,
            "Add a character to the list of characters to check collision against.")
        .def("remove", &CharacterVsCharacterCollisionSimple::Remove, "character"_a,
            "Remove a character from the list of characters to check collision against.")
        .def("collide_character", &CharacterVsCharacterCollisionSimple::CollideCharacter, "character"_a, "center_of_mass_transform"_a, "collide_shape_settings"_a, "base_offset"_a, "collector"_a)
        .def("cast_character", &CharacterVsCharacterCollisionSimple::CastCharacter, "character"_a, "center_of_mass_transform"_a, "direction"_a, "shape_cast_settings"_a, "base_offset"_a, "collector"_a)
        .def_rw("characters", &CharacterVsCharacterCollisionSimple::mCharacters,
            "The list of characters to check collision against");

    nb::class_<CharacterVirtual::ExtendedUpdateSettings> extendedUpdateSettingsCls(characterVirtualCls, "ExtendedUpdateSettings",
        "Settings struct with settings for ExtendedUpdate");
    extendedUpdateSettingsCls
        .def_rw("stick_to_floor_step_down", &CharacterVirtual::ExtendedUpdateSettings::mStickToFloorStepDown,
            "See StickToFloor inStepDown parameter. Can be zero to turn off.")
        .def_rw("walk_stairs_step_up", &CharacterVirtual::ExtendedUpdateSettings::mWalkStairsStepUp,
            "See WalkStairs inStepUp parameter. Can be zero to turn off.")
        .def_rw("walk_stairs_min_step_forward", &CharacterVirtual::ExtendedUpdateSettings::mWalkStairsMinStepForward,
            "See WalkStairs inStepForward parameter. Note that the parameter only indicates a magnitude, direction is taken from current velocity.")
        .def_rw("walk_stairs_step_forward_test", &CharacterVirtual::ExtendedUpdateSettings::mWalkStairsStepForwardTest,
            "See WalkStairs inStepForwardTest parameter. Note that the parameter only indicates a magnitude, direction is taken from current velocity.")
        .def_rw("walk_stairs_cos_angle_forward_contact", &CharacterVirtual::ExtendedUpdateSettings::mWalkStairsCosAngleForwardContact,
            "Cos(angle) where angle is the maximum angle between the ground normal in the horizontal plane and the character forward vector where we're willing to adjust the step forward test towards the contact normal.")
        .def_rw("walk_stairs_step_down_extra", &CharacterVirtual::ExtendedUpdateSettings::mWalkStairsStepDownExtra,
            "See WalkStairs inStepDownExtra");

    nb::class_<CharacterVirtual::Contact> contactCls(characterVirtualCls, "Contact");
    contactCls
        .def("save_state", &CharacterVirtual::Contact::SaveState, "stream"_a)
        .def("restore_state", &CharacterVirtual::Contact::RestoreState, "stream"_a)
        .def("is_same_body", &CharacterVirtual::Contact::IsSameBody, "other"_a, "Checks if two contacts refer to the same body (or virtual character)")
        .def_rw("position", &CharacterVirtual::Contact::mPosition,
            "Position where the character makes contact")
        .def_rw("linear_velocity", &CharacterVirtual::Contact::mLinearVelocity,
            "Velocity of the contact point")
        .def_rw("contact_normal", &CharacterVirtual::Contact::mContactNormal,
            "CharacterVirtual::Contact normal, pointing towards the character")
        .def_rw("surface_normal", &CharacterVirtual::Contact::mSurfaceNormal,
            "Surface normal of the contact")
        .def_rw("distance", &CharacterVirtual::Contact::mDistance,
            "Distance to the contact <= 0 means that it is an actual contact, > 0 means predictive")
        .def_rw("fraction", &CharacterVirtual::Contact::mFraction,
            "Fraction along the path where this contact takes place")
        .def_rw("body_b", &CharacterVirtual::Contact::mBodyB,
            "ID of body we're colliding with (if not invalid)")
        .def_rw("character_b", &CharacterVirtual::Contact::mCharacterB,
            "Character we're colliding with (if not null)")
        .def_rw("sub_shape_idb", &CharacterVirtual::Contact::mSubShapeIDB,
            "Sub shape ID of body we're colliding with")
        .def_rw("motion_type_b", &CharacterVirtual::Contact::mMotionTypeB,
            "Motion type of B, used to determine the priority of the contact")
        .def_rw("is_sensor_b", &CharacterVirtual::Contact::mIsSensorB,
            "If B is a sensor")
        .def_rw("user_data", &CharacterVirtual::Contact::mUserData,
            "User data of B")
        .def_rw("material", &CharacterVirtual::Contact::mMaterial,
            "Material of B")
        .def_rw("had_collision", &CharacterVirtual::Contact::mHadCollision,
            "If the character actually collided with the contact (can be false if a predictive contact never becomes a real one)")
        .def_rw("was_discarded", &CharacterVirtual::Contact::mWasDiscarded,
            "If the contact validate callback chose to discard this contact")
        .def_rw("can_push_character", &CharacterVirtual::Contact::mCanPushCharacter,
            "When true, the velocity of the contact point can push the character");
}