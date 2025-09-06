#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/LargeIslandSplitter.h>
#include <Jolt/Physics/IslandBuilder.h>
#include <Jolt/Physics/Body/BodyManager.h>

#include <nanobind/stl/function.h>

void BindVehicleConstraint(nb::module_ &m) {
    nb::class_<VehicleConstraintSettings, ConstraintSettings> vehicleConstraintSettingsCls(m, "VehicleConstraintSettings",
        "Configuration for constraint that simulates a wheeled vehicle.\n"
        "The properties in this constraint are largely based on \"Car Physics for Games\" by Marco Monster.\n"
        "See: https://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html");
    vehicleConstraintSettingsCls
        .def("save_binary_state", &VehicleConstraintSettings::SaveBinaryState, "stream"_a,
            "Saves the contents of the constraint settings in binary form to inStream.")
        .def_rw("up", &VehicleConstraintSettings::mUp,
            "Vector indicating the up direction of the vehicle (in local space to the body)")
        .def_rw("forward", &VehicleConstraintSettings::mForward,
            "Vector indicating forward direction of the vehicle (in local space to the body)")
        .def_rw("max_pitch_roll_angle", &VehicleConstraintSettings::mMaxPitchRollAngle,
            "Defines the maximum pitch/roll angle (rad), can be used to avoid the car from getting upside down. The vehicle up direction will stay within a cone centered around the up axis with half top angle mMaxPitchRollAngle, set to pi to turn off.")
        .def_rw("wheels", &VehicleConstraintSettings::mWheels,
            "List of wheels and their properties")
        .def_rw("anti_roll_bars", &VehicleConstraintSettings::mAntiRollBars,
            "List of anti rollbars and their properties")
        .def_rw("controller", &VehicleConstraintSettings::mController,
            "Defines how the vehicle can accelerate / decelerate");

    nb::class_<VehicleConstraint, Constraint> vehicleConstraintCls(m, "VehicleConstraint",
        "Constraint that simulates a vehicle\n"
        "Note: Don't forget to register the constraint as a StepListener with the PhysicsSystem!\n"
        "When the vehicle drives over very light objects (rubble) you may see the car body dip down. This is a known issue and is an artifact of the iterative solver that Jolt is using.\n"
        "Basically if a light object is sandwiched between two heavy objects (the static floor and the car body), the light object is not able to transfer enough force from the ground to\n"
        "the car body to keep the car body up. You can see this effect in the HeavyOnLightTest sample, the boxes on the right have a lot of penetration because they're on top of light objects.\n"
        "There are a couple of ways to improve this:\n"
        "1. You can increase the number of velocity steps (global settings PhysicsSettings::mNumVelocitySteps or if you only want to increase it on\n"
        "the vehicle you can use VehicleConstraintSettings::mNumVelocityStepsOverride). E.g. going from 10 to 30 steps in the HeavyOnLightTest sample makes the penetration a lot less.\n"
        "The number of position steps can also be increased (the first prevents the body from going down, the second corrects it if the problem did\n"
        "occur which inevitably happens due to numerical drift). This solution costs CPU cycles.\n"
        "2. You can reduce the mass difference between the vehicle body and the rubble on the floor (by making the rubble heavier or the car lighter).\n"
        "3. You could filter out collisions between the vehicle collision test and the rubble completely. This would make the wheels ignore the rubble but would cause the vehicle to drive\n"
        "through it as if nothing happened. You could create fake wheels (keyframed bodies) that move along with the vehicle and that only collide with rubble (and not the vehicle or the ground).\n"
        "This would cause the vehicle to push away the rubble without the rubble being able to affect the vehicle (unless it hits the main body of course).\n"
        "Note that when driving over rubble, you may see the wheel jump up and down quite quickly because one frame a collision is found and the next frame not.\n"
        "To alleviate this, it may be needed to smooth the motion of the visual mesh for the wheel.");
    vehicleConstraintCls
        // .def(nb::init<>(), "vehicle_body"_a, "settings"_a,
        //     "Constructor / destructor")
        .def("get_sub_type", &VehicleConstraint::GetSubType,
            "Get the type of a constraint")
        .def("set_max_pitch_roll_angle", &VehicleConstraint::SetMaxPitchRollAngle, "max_pitch_roll_angle"_a,
            "Defines the maximum pitch/roll angle (rad), can be used to avoid the car from getting upside down. The vehicle up direction will stay within a cone centered around the up axis with half top angle mMaxPitchRollAngle, set to pi to turn off.")
        .def("set_vehicle_collision_tester", &VehicleConstraint::SetVehicleCollisionTester, "tester"_a,
            "Set the interface that tests collision between wheel and ground")
        .def("set_combine_friction", &VehicleConstraint::SetCombineFriction, "combine_friction"_a,
            "Set the function that combines the friction of two bodies and returns it\n"
            "Default method is the geometric mean: sqrt(friction1 * friction2).")
        .def("get_combine_friction", &VehicleConstraint::GetCombineFriction)
        .def("get_pre_step_callback", &VehicleConstraint::GetPreStepCallback,
            "Callback function to notify that PhysicsStepListener::OnStep has started for this vehicle. Default is to do nothing.\n"
            "Can be used to allow higher-level code to e.g. control steering. This is the last moment that the position/orientation of the vehicle can be changed.\n"
            "Wheel collision checks have not been performed yet.")
        .def("set_pre_step_callback", &VehicleConstraint::SetPreStepCallback, "pre_step_callback"_a)
        .def("get_post_collide_callback", &VehicleConstraint::GetPostCollideCallback,
            "Callback function to notify that PhysicsStepListener::OnStep has just completed wheel collision checks. Default is to do nothing.\n"
            "Can be used to allow higher-level code to e.g. detect tire contact or to modify the velocity of the vehicle based on the wheel contacts.\n"
            "You should not change the position of the vehicle in this callback as the wheel collision checks have already been performed.")
        .def("set_post_collide_callback", &VehicleConstraint::SetPostCollideCallback, "post_collide_callback"_a)
        .def("get_post_step_callback", &VehicleConstraint::GetPostStepCallback,
            "Callback function to notify that PhysicsStepListener::OnStep has completed for this vehicle. Default is to do nothing.\n"
            "Can be used to allow higher-level code to e.g. control the vehicle in the air.\n"
            "You should not change the position of the vehicle in this callback as the wheel collision checks have already been performed.")
        .def("set_post_step_callback", &VehicleConstraint::SetPostStepCallback, "post_step_callback"_a)
        .def("override_gravity", &VehicleConstraint::OverrideGravity, "gravity"_a,
            "Override gravity for this vehicle. Note that overriding gravity will set the gravity factor of the vehicle body to 0 and apply gravity in the PhysicsStepListener instead.")
        .def("is_gravity_overridden", &VehicleConstraint::IsGravityOverridden)
        .def("get_gravity_override", &VehicleConstraint::GetGravityOverride)
        .def("reset_gravity_override", &VehicleConstraint::ResetGravityOverride)
        .def("get_local_forward", &VehicleConstraint::GetLocalForward,
            "Get the local space forward vector of the vehicle")
        .def("get_local_up", &VehicleConstraint::GetLocalUp,
            "Get the local space up vector of the vehicle")
        .def("get_world_up", &VehicleConstraint::GetWorldUp,
            "Vector indicating the world space up direction (used to limit vehicle pitch/roll), calculated every frame by inverting gravity")
        .def("get_vehicle_body", &VehicleConstraint::GetVehicleBody, nb::rv_policy::reference,
            "Access to the vehicle body")
        .def("get_controller", nb::overload_cast<>(&VehicleConstraint::GetController), nb::rv_policy::reference,
            "Access to the vehicle controller interface (determines acceleration / deceleration)")
        .def("get_wheels", nb::overload_cast<>(&VehicleConstraint::GetWheels),
            "Get the state of a wheels (writable interface, allows you to make changes to the configuration which will take effect the next time step)")
        .def("get_wheel", nb::overload_cast<uint>(&VehicleConstraint::GetWheel), "idx"_a, nb::rv_policy::reference,
            "Get the state of a wheel")
        .def("get_wheel_local_basis", &VehicleConstraint::GetWheelLocalBasis, "wheel"_a, "forward"_a, "up"_a, "right"_a,
            "Get the basis vectors for the wheel in local space to the vehicle body (note: basis does not rotate when the wheel rotates around its axis).\n"
            "Args:\n"
            "    wheel (Wheel*): Wheel to fetch basis for.\n"
            "    forward (Vec3): Forward vector for the wheel.\n"
            "    up (Vec3): Up vector for the wheel.\n"
            "    right (Vec3): Right vector for the wheel.")
        .def("get_wheel_local_transform", &VehicleConstraint::GetWheelLocalTransform, "wheel_index"_a, "wheel_right"_a, "wheel_up"_a,
            "Get the transform of a wheel in local space to the vehicle body, returns a matrix that transforms a cylinder aligned with the Y axis in body space (not COM space).\n"
            "Args:\n"
            "    wheel_index (int): Index of the wheel to fetch.\n"
            "    wheel_right (Vec3): Unit vector that indicates right in model space of the wheel (so if you only have 1 wheel model, you probably want to specify the opposite direction for the left and right wheels).\n"
            "    wheel_up (Vec3): Unit vector that indicates up in model space of the wheel.")
        .def("get_wheel_world_transform", &VehicleConstraint::GetWheelWorldTransform, "wheel_index"_a, "wheel_right"_a, "wheel_up"_a,
            "Get the transform of a wheel in world space, returns a matrix that transforms a cylinder aligned with the Y axis in world space.\n"
            "Args:\n"
            "    wheel_index (int): Index of the wheel to fetch.\n"
            "    wheel_right (Vec3): Unit vector that indicates right in model space of the wheel (so if you only have 1 wheel model, you probably want to specify the opposite direction for the left and right wheels).\n"
            "    wheel_up (Vec3): Unit vector that indicates up in model space of the wheel.")
        .def("set_num_steps_between_collision_test_active", &VehicleConstraint::SetNumStepsBetweenCollisionTestActive, "steps"_a,
            "Number of simulation steps between wheel collision tests when the vehicle is active. Default is 1. 0 = never, 1 = every step, 2 = every other step, etc.\n"
            "Note that if a vehicle has multiple wheels and the number of steps > 1, the wheels will be tested in a round robin fashion.\n"
            "If there are multiple vehicles, the tests will be spread out based on the BodyID of the vehicle.\n"
            "If you set this to test less than every step, you may see simulation artifacts. This setting can be used to reduce the cost of simulating vehicles in the distance.")
        .def("get_num_steps_between_collision_test_active", &VehicleConstraint::GetNumStepsBetweenCollisionTestActive)
        .def("set_num_steps_between_collision_test_inactive", &VehicleConstraint::SetNumStepsBetweenCollisionTestInactive, "steps"_a,
            "Number of simulation steps between wheel collision tests when the vehicle is inactive. Default is 1. 0 = never, 1 = every step, 2 = every other step, etc.\n"
            "Note that if a vehicle has multiple wheels and the number of steps > 1, the wheels will be tested in a round robin fashion.\n"
            "If there are multiple vehicles, the tests will be spread out based on the BodyID of the vehicle.\n"
            "This number can be lower than the number of steps when the vehicle is active as the only purpose of this test is\n"
            "to allow the vehicle to wake up in response to bodies moving into the wheels but not touching the body of the vehicle.")
        .def("get_num_steps_between_collision_test_inactive", &VehicleConstraint::GetNumStepsBetweenCollisionTestInactive)
        .def("is_active", &VehicleConstraint::IsActive)
        .def("notify_shape_changed", &VehicleConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &VehicleConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &VehicleConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &VehicleConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &VehicleConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &VehicleConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("build_islands", &VehicleConstraint::BuildIslands, "constraint_index"_a, "builder"_a, "body_manager"_a)
        .def("build_island_splits", &VehicleConstraint::BuildIslandSplits, "splitter"_a)
        .def("draw_constraint", &VehicleConstraint::DrawConstraint, "renderer"_a)
        .def("draw_constraint_limits", &VehicleConstraint::DrawConstraintLimits, "renderer"_a)
        .def("save_state", &VehicleConstraint::SaveState, "stream"_a)
        .def("restore_state", &VehicleConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &VehicleConstraint::GetConstraintSettings);
}