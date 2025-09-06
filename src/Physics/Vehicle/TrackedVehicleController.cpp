#include "Common.h"
#include <Jolt/Physics/Vehicle/TrackedVehicleController.h>

void BindTrackedVehicleController(nb::module_ &m) {
    nb::class_<WheelSettingsTV, WheelSettings> wheelSettingsTVCls(m, "WheelSettingsTV",
        "WheelSettings object specifically for TrackedVehicleController");
    wheelSettingsTVCls
        .def("save_binary_state", &WheelSettingsTV::SaveBinaryState, "stream"_a)
        .def("restore_binary_state", &WheelSettingsTV::RestoreBinaryState, "stream"_a)
        .def_rw("longitudinal_friction", &WheelSettingsTV::mLongitudinalFriction,
            "Friction in forward direction of tire")
        .def_rw("lateral_friction", &WheelSettingsTV::mLateralFriction,
            "Friction in sideway direction of tire");

    nb::class_<WheelTV, Wheel> wheelTVCls(m, "WheelTV",
        "Wheel object specifically for TrackedVehicleController");
    wheelTVCls
        .def(nb::init<const WheelSettingsTV &>(), "wheel"_a,
            "Constructor")
        .def("get_settings", &WheelTV::GetSettings, nb::rv_policy::reference,
            "Override GetSettings and cast to the correct class")
        .def("calculate_angular_velocity", &WheelTV::CalculateAngularVelocity, "constraint"_a,
            "Update the angular velocity of the wheel based on the angular velocity of the track")
        .def("update", &WheelTV::Update, "wheel_index"_a, "delta_time"_a, "constraint"_a,
            "Update the wheel rotation based on the current angular velocity")
        .def_rw("track_index", &WheelTV::mTrackIndex,
            "Index in mTracks to which this wheel is attached (calculated on initialization)")
        .def_rw("combined_longitudinal_friction", &WheelTV::mCombinedLongitudinalFriction,
            "Combined friction coefficient in longitudinal direction (combines terrain and track)")
        .def_rw("combined_lateral_friction", &WheelTV::mCombinedLateralFriction,
            "Combined friction coefficient in lateral direction (combines terrain and track)")
        .def_rw("brake_impulse", &WheelTV::mBrakeImpulse,
            "Amount of impulse that the brakes can apply to the floor (excluding friction), spread out from brake impulse applied on track");

    nb::class_<TrackedVehicleControllerSettings, VehicleControllerSettings> trackedVehicleControllerSettingsCls(m, "TrackedVehicleControllerSettings",
        "Settings of a vehicle with tank tracks\n"
        "Default settings are based around what I could find about the M1 Abrams tank.\n"
        "Note to avoid issues with very heavy objects vs very light objects the mass of the tank should be a lot lower (say 10x) than that of a real tank. That means that the engine/brake torque is also 10x less.");
    trackedVehicleControllerSettingsCls
        .def(nb::init<>())
        .def("construct_controller", &TrackedVehicleControllerSettings::ConstructController, "constraint"_a, nb::rv_policy::reference)
        .def("save_binary_state", &TrackedVehicleControllerSettings::SaveBinaryState, "stream"_a)
        .def("restore_binary_state", &TrackedVehicleControllerSettings::RestoreBinaryState, "stream"_a)
        .def_rw("engine", &TrackedVehicleControllerSettings::mEngine,
            "The properties of the engine")
        .def_rw("transmission", &TrackedVehicleControllerSettings::mTransmission,
            "The properties of the transmission (aka gear box)")
        // .def_rw("tracks", &TrackedVehicleControllerSettings::mTracks,        // TODO:
        //     "List of tracks and their properties")
            ;

    nb::class_<TrackedVehicleController, VehicleController> trackedVehicleControllerCls(m, "TrackedVehicleController",
        "Runtime controller class for vehicle with tank tracks");
    trackedVehicleControllerCls
        .def(nb::init<const TrackedVehicleControllerSettings &, VehicleConstraint &>(), "settings"_a, "constraint"_a,
            "Constructor")
        .def("set_driver_input", &TrackedVehicleController::SetDriverInput, "forward"_a, "left_ratio"_a, "right_ratio"_a, "brake"_a,
            "Set input from driver.\n"
            "Args:\n"
            "    forward (float): Value between -1 and 1 for auto transmission and value between 0 and 1 indicating desired driving direction and amount the gas pedal is pressed.\n"
            "    left_ratio (float): Value between -1 and 1 indicating an extra multiplier to the rotation rate of the left track (used for steering).\n"
            "    right_ratio (float): Value between -1 and 1 indicating an extra multiplier to the rotation rate of the right track (used for steering).\n"
            "    brake (float): Value between 0 and 1 indicating how strong the brake pedal is pressed.")
        .def("set_forward_input", &TrackedVehicleController::SetForwardInput, "forward"_a,
            "Value between -1 and 1 for auto transmission and value between 0 and 1 indicating desired driving direction and amount the gas pedal is pressed")
        .def("get_forward_input", &TrackedVehicleController::GetForwardInput)
        .def("set_left_ratio", &TrackedVehicleController::SetLeftRatio, "left_ratio"_a,
            "Value between -1 and 1 indicating an extra multiplier to the rotation rate of the left track (used for steering)")
        .def("get_left_ratio", &TrackedVehicleController::GetLeftRatio)
        .def("set_right_ratio", &TrackedVehicleController::SetRightRatio, "right_ratio"_a,
            "Value between -1 and 1 indicating an extra multiplier to the rotation rate of the right track (used for steering)")
        .def("get_right_ratio", &TrackedVehicleController::GetRightRatio)
        .def("set_brake_input", &TrackedVehicleController::SetBrakeInput, "brake"_a,
            "Value between 0 and 1 indicating how strong the brake pedal is pressed")
        .def("get_brake_input", &TrackedVehicleController::GetBrakeInput)
        .def("get_engine", nb::overload_cast<>(&TrackedVehicleController::GetEngine),
            "Get current engine state (writable interface, allows you to make changes to the configuration which will take effect the next time step)")
        .def("get_transmission", nb::overload_cast<>(&TrackedVehicleController::GetTransmission),
            "Get current transmission state (writable interface, allows you to make changes to the configuration which will take effect the next time step)")
        .def("get_tracks", nb::overload_cast<>(&TrackedVehicleController::GetTracks),
            "Get the tracks this vehicle has (writable interface, allows you to make changes to the configuration which will take effect the next time step)")
        .def("set_rpm_meter", &TrackedVehicleController::SetRPMMeter, "position"_a, "size"_a,
            "Debug drawing of RPM meter");
}
