#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleTransmission.h>

void BindVehicleTransmission(nb::module_ &m) {
    nb::enum_<ETransmissionMode>(m, "ETransmissionMode",
        "How gears are shifted")
        .value("AUTO", ETransmissionMode::Auto,
            "Automatically shift gear up and down")
        .value("MANUAL", ETransmissionMode::Manual,
            "Manual gear shift (call SetTransmissionInput)");
    nb::class_<VehicleTransmissionSettings> vehicleTransmissionSettingsCls(m, "VehicleTransmissionSettings",
        "Configuration for the transmission of a vehicle (gear box)");
    vehicleTransmissionSettingsCls
        .def("save_binary_state", &VehicleTransmissionSettings::SaveBinaryState, "stream"_a,
            "Saves the contents in binary form to inStream.")
        .def("restore_binary_state", &VehicleTransmissionSettings::RestoreBinaryState, "stream"_a,
            "Restores the contents in binary form to inStream.")
        .def_rw("mode", &VehicleTransmissionSettings::mMode,
            "How to switch gears")
        .def_rw("gear_ratios", &VehicleTransmissionSettings::mGearRatios,
            "Ratio in rotation rate between engine and gear box, first element is 1st gear, 2nd element 2nd gear etc.")
        .def_rw("reverse_gear_ratios", &VehicleTransmissionSettings::mReverseGearRatios,
            "Ratio in rotation rate between engine and gear box when driving in reverse")
        .def_rw("switch_time", &VehicleTransmissionSettings::mSwitchTime,
            "How long it takes to switch gears (s), only used in auto mode")
        .def_rw("clutch_release_time", &VehicleTransmissionSettings::mClutchReleaseTime,
            "How long it takes to release the clutch (go to full friction), only used in auto mode")
        .def_rw("switch_latency", &VehicleTransmissionSettings::mSwitchLatency,
            "How long to wait after releasing the clutch before another switch is attempted (s), only used in auto mode")
        .def_rw("shift_up_rpm", &VehicleTransmissionSettings::mShiftUpRPM,
            "If RPM of engine is bigger then this we will shift a gear up, only used in auto mode")
        .def_rw("shift_down_rpm", &VehicleTransmissionSettings::mShiftDownRPM,
            "If RPM of engine is smaller then this we will shift a gear down, only used in auto mode")
        .def_rw("clutch_strength", &VehicleTransmissionSettings::mClutchStrength,
            "Strength of the clutch when fully engaged. Total torque a clutch applies is Torque = ClutchStrength * (Velocity Engine - Avg Velocity Wheels At Clutch) (units: k m^2 s^-1)");

    nb::class_<VehicleTransmission, VehicleTransmissionSettings> vehicleTransmissionCls(m, "VehicleTransmission",
        "Runtime data for transmission");
    vehicleTransmissionCls
        .def("set", &VehicleTransmission::Set, "current_gear"_a, "clutch_friction"_a,
            "Set input from driver regarding the transmission (only relevant when transmission is set to manual mode).\n"
            "Args:\n"
            "    current_gear (int): Current gear, -1 = reverse, 0 = neutral, 1 = 1st gear etc.\n"
            "    clutch_friction (float): Value between 0 and 1 indicating how much friction the clutch gives (0 = no friction, 1 = full friction).")
        .def("update", &VehicleTransmission::Update, "delta_time"_a, "current_rpm"_a, "forward_input"_a, "can_shift_up"_a,
            "Update the current gear and clutch friction if the transmission is in auto mode.\n"
            "Args:\n"
            "    delta_time (float): Time step delta time in s.\n"
            "    current_rpm (float): Current RPM for engine.\n"
            "    forward_input (float): Hint if the user wants to drive forward (> 0) or backwards (< 0).\n"
            "    can_shift_up (bool): Indicates if we want to allow the transmission to shift up (e.g. pass false if wheels are slipping).")
        .def("get_current_gear", &VehicleTransmission::GetCurrentGear,
            "Current gear, -1 = reverse, 0 = neutral, 1 = 1st gear etc.")
        .def("get_clutch_friction", &VehicleTransmission::GetClutchFriction,
            "Value between 0 and 1 indicating how much friction the clutch gives (0 = no friction, 1 = full friction)")
        .def("is_switching_gear", &VehicleTransmission::IsSwitchingGear,
            "If the auto box is currently switching gears")
        .def("get_current_ratio", &VehicleTransmission::GetCurrentRatio,
            "Return the transmission ratio based on the current gear (ratio between engine and differential)")
        .def("allow_sleep", &VehicleTransmission::AllowSleep,
            "Only allow sleeping when the transmission is idle")
        .def("save_state", &VehicleTransmission::SaveState, "stream"_a,
            "Saving state for replay")
        .def("restore_state", &VehicleTransmission::RestoreState, "stream"_a);
}