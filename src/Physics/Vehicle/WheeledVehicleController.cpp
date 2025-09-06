#include "Common.h"
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>

#include<nanobind/stl/function.h>

void BindWheeledVehicleController(nb::module_ &m) {
    nb::class_<WheelSettingsWV, WheelSettings> wheelSettingsWVCls(m, "WheelSettingsWV",
        "WheelSettings object specifically for WheeledVehicleController");
    wheelSettingsWVCls
        .def(nb::init<>(),
            "Constructor")
        .def("save_binary_state", &WheelSettingsWV::SaveBinaryState, "stream"_a)
        .def("restore_binary_state", &WheelSettingsWV::RestoreBinaryState, "stream"_a)
        .def_rw("inertia", &WheelSettingsWV::mInertia,
            "Moment of inertia (kg m^2), for a cylinder this would be 0.5 * M * R^2 which is 0.9 for a wheel with a mass of 20 kg and radius 0.3 m")
        .def_rw("angular_damping", &WheelSettingsWV::mAngularDamping,
            "Angular damping factor of the wheel: dw/dt = -c * w")
        .def_rw("max_steer_angle", &WheelSettingsWV::mMaxSteerAngle,
            "How much this wheel can steer (radians)")
        .def_rw("longitudinal_friction", &WheelSettingsWV::mLongitudinalFriction,
            "On the Y-axis: friction in the forward direction of the tire. Friction is normally between 0 (no friction) and 1 (full friction) although friction can be a little bit higher than 1 because of the profile of a tire. On the X-axis: the slip ratio (fraction) defined as (omega_wheel * r_wheel - v_longitudinal) / |v_longitudinal|. You can see slip ratio as the amount the wheel is spinning relative to the floor: 0 means the wheel has full traction and is rolling perfectly in sync with the ground, 1 is for example when the wheel is locked and sliding over the ground.")
        .def_rw("lateral_friction", &WheelSettingsWV::mLateralFriction,
            "On the Y-axis: friction in the sideways direction of the tire. Friction is normally between 0 (no friction) and 1 (full friction) although friction can be a little bit higher than 1 because of the profile of a tire. On the X-axis: the slip angle (degrees) defined as angle between relative contact velocity and tire direction.")
        .def_rw("max_brake_torque", &WheelSettingsWV::mMaxBrakeTorque,
            "How much torque (Nm) the brakes can apply to this wheel")
        .def_rw("max_hand_brake_torque", &WheelSettingsWV::mMaxHandBrakeTorque,
            "How much torque (Nm) the hand brake can apply to this wheel (usually only applied to the rear wheels)");

    nb::class_<WheelWV, Wheel> wheelWVCls(m, "WheelWV",
        "Wheel object specifically for WheeledVehicleController");
    wheelWVCls
        .def(nb::init<const WheelSettingsWV &>(), "wheel"_a,
            "Constructor")
        .def("get_settings", &WheelWV::GetSettings, nb::rv_policy::reference,
            "Override GetSettings and cast to the correct class")
        .def("apply_torque", &WheelWV::ApplyTorque, "torque"_a, "delta_time"_a,
            "Apply a torque (N m) to the wheel for a particular delta time")
        .def("update", &WheelWV::Update, "wheel_index"_a, "delta_time"_a, "constraint"_a,
            "Update the wheel rotation based on the current angular velocity")
        .def_rw("longitudinal_slip", &WheelWV::mLongitudinalSlip,
            "Velocity difference between ground and wheel relative to ground velocity")
        .def_rw("lateral_slip", &WheelWV::mLateralSlip,
            "Angular difference (in radians) between ground and wheel relative to ground velocity")
        .def_rw("combined_longitudinal_friction", &WheelWV::mCombinedLongitudinalFriction,
            "Combined friction coefficient in longitudinal direction (combines terrain and tires)")
        .def_rw("combined_lateral_friction", &WheelWV::mCombinedLateralFriction,
            "Combined friction coefficient in lateral direction (combines terrain and tires)")
        .def_rw("brake_impulse", &WheelWV::mBrakeImpulse,
            "Amount of impulse that the brakes can apply to the floor (excluding friction)");

    nb::class_<WheeledVehicleControllerSettings, VehicleControllerSettings> wheeledVehicleControllerSettingsCls(m, "WheeledVehicleControllerSettings",
        "Settings of a vehicle with regular wheels\n"
        "The properties in this controller are largely based on \"Car Physics for Games\" by Marco Monster.\n"
        "See: https://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html");
    wheeledVehicleControllerSettingsCls
        .def("construct_controller", &WheeledVehicleControllerSettings::ConstructController, "constraint"_a, nb::rv_policy::reference)
        .def("save_binary_state", &WheeledVehicleControllerSettings::SaveBinaryState, "stream"_a)
        .def("restore_binary_state", &WheeledVehicleControllerSettings::RestoreBinaryState, "stream"_a)
        .def_rw("engine", &WheeledVehicleControllerSettings::mEngine,
            "The properties of the engine")
        .def_rw("transmission", &WheeledVehicleControllerSettings::mTransmission,
            "The properties of the transmission (aka gear box)")
        .def_rw("differentials", &WheeledVehicleControllerSettings::mDifferentials,
            "List of differentials and their properties")
        .def_rw("differential_limited_slip_ratio", &WheeledVehicleControllerSettings::mDifferentialLimitedSlipRatio,
            "Ratio max / min average wheel speed of each differential (measured at the clutch). When the ratio is exceeded all torque gets distributed to the differential with the minimal average velocity. This allows implementing a limited slip differential between differentials. Set to FLT_MAX for an open differential. Value should be > 1.");

    nb::class_<WheeledVehicleController, VehicleController> wheeledVehicleControllerCls(m, "WheeledVehicleController",
        "Runtime controller class");
    wheeledVehicleControllerCls
        .def(nb::init<const WheeledVehicleControllerSettings &, VehicleConstraint &>(), "settings"_a, "constraint"_a,
            "Constructor")
        .def("set_driver_input", &WheeledVehicleController::SetDriverInput, "forward"_a, "right"_a, "brake"_a, "hand_brake"_a,
            "Set input from driver.\n"
            "Args:\n"
            "    forward (float): Value between -1 and 1 for auto transmission and value between 0 and 1 indicating desired driving direction and amount the gas pedal is pressed.\n"
            "    right (float): Value between -1 and 1 indicating desired steering angle (1 = right).\n"
            "    brake (float): Value between 0 and 1 indicating how strong the brake pedal is pressed.\n"
            "    hand_brake (float): Value between 0 and 1 indicating how strong the hand brake is pulled.")
        .def("set_forward_input", &WheeledVehicleController::SetForwardInput, "forward"_a,
            "Value between -1 and 1 for auto transmission and value between 0 and 1 indicating desired driving direction and amount the gas pedal is pressed")
        .def("get_forward_input", &WheeledVehicleController::GetForwardInput)
        .def("set_right_input", &WheeledVehicleController::SetRightInput, "right"_a,
            "Value between -1 and 1 indicating desired steering angle (1 = right)")
        .def("get_right_input", &WheeledVehicleController::GetRightInput)
        .def("set_brake_input", &WheeledVehicleController::SetBrakeInput, "brake"_a,
            "Value between 0 and 1 indicating how strong the brake pedal is pressed")
        .def("get_brake_input", &WheeledVehicleController::GetBrakeInput)
        .def("set_hand_brake_input", &WheeledVehicleController::SetHandBrakeInput, "hand_brake"_a,
            "Value between 0 and 1 indicating how strong the hand brake is pulled")
        .def("get_hand_brake_input", &WheeledVehicleController::GetHandBrakeInput)
        .def("get_engine", nb::overload_cast<>(&WheeledVehicleController::GetEngine),
            "Get current engine state (writable interface, allows you to make changes to the configuration which will take effect the next time step)")
        .def("get_transmission", nb::overload_cast<>(&WheeledVehicleController::GetTransmission),
            "Get current transmission state (writable interface, allows you to make changes to the configuration which will take effect the next time step)")
        .def("get_differentials", nb::overload_cast<>(&WheeledVehicleController::GetDifferentials),
            "Get the differentials this vehicle has (writable interface, allows you to make changes to the configuration which will take effect the next time step)")
        .def("get_differential_limited_slip_ratio", &WheeledVehicleController::GetDifferentialLimitedSlipRatio,
            "Ratio max / min average wheel speed of each differential (measured at the clutch).")
        .def("set_differential_limited_slip_ratio", &WheeledVehicleController::SetDifferentialLimitedSlipRatio, "v"_a)
        .def("get_wheel_speed_at_clutch", &WheeledVehicleController::GetWheelSpeedAtClutch,
            "Get the average wheel speed of all driven wheels (measured at the clutch)")
        .def("get_tire_max_impulse_callback", &WheeledVehicleController::GetTireMaxImpulseCallback,
            "Calculate max tire impulses by combining friction, slip, and suspension impulse. Note that the actual applied impulse may be lower (e.g. when the vehicle is stationary on a horizontal surface the actual impulse applied will be 0).")
        .def("set_tire_max_impulse_callback", &WheeledVehicleController::SetTireMaxImpulseCallback, "tire_max_impulse_callback"_a)
        .def("set_rpm_meter", &WheeledVehicleController::SetRPMMeter, "position"_a, "size"_a,
            "Debug drawing of RPM meter");
}