#include "Common.h"
#include <Jolt/Physics/Constraints/MotorSettings.h>
#include <Jolt/Core/StreamIn.h>
#include <Jolt/Core/StreamOut.h>

void BindMotorSettings(nb::module_ &m) {
    nb::enum_<EMotorState>(m, "EMotorState")
        .value("OFF", EMotorState::Off,
            "Motor is off")
        .value("VELOCITY", EMotorState::Velocity,
            "Motor will drive to target velocity")
        .value("POSITION", EMotorState::Position,
            "Motor will drive to target position");
    nb::class_<MotorSettings> motorSettingsCls(m, "MotorSettings",
        "Class that contains the settings for a constraint motor.\n"
        "See the main page of the API documentation for more information on how to configure a motor.");
    motorSettingsCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const MotorSettings &>())
        .def("assign", &MotorSettings::operator=)
        .def(nb::init<float, float>(), "frequency"_a, "damping"_a)
        .def(nb::init<float, float, float, float>(), "frequency"_a, "damping"_a, "force_limit"_a, "torque_limit"_a)
        .def("set_force_limits", &MotorSettings::SetForceLimits, "min"_a, "max"_a,
            "Set asymmetric force limits")
        .def("set_torque_limits", &MotorSettings::SetTorqueLimits, "min"_a, "max"_a,
            "Set asymmetric torque limits")
        .def("set_force_limit", &MotorSettings::SetForceLimit, "limit"_a,
            "Set symmetric force limits")
        .def("set_torque_limit", &MotorSettings::SetTorqueLimit, "limit"_a,
            "Set symmetric torque limits")
        .def("is_valid", &MotorSettings::IsValid,
            "Check if settings are valid")
        .def("save_binary_state", &MotorSettings::SaveBinaryState, "stream"_a,
            "Saves the contents of the motor settings in binary form to inStream.")
        .def("restore_binary_state", &MotorSettings::RestoreBinaryState, "stream"_a,
            "Restores contents from the binary stream inStream.")
        .def_rw("spring_settings", &MotorSettings::mSpringSettings,
            "Settings for the spring that is used to drive to the position target (not used when motor is a velocity motor).")
        .def_rw("min_force_limit", &MotorSettings::mMinForceLimit,
            "Minimum force to apply in case of a linear constraint (N). Usually this is -mMaxForceLimit unless you want a motor that can e.g. push but not pull. Not used when motor is an angular motor.")
        .def_rw("max_force_limit", &MotorSettings::mMaxForceLimit,
            "Maximum force to apply in case of a linear constraint (N). Not used when motor is an angular motor.")
        .def_rw("min_torque_limit", &MotorSettings::mMinTorqueLimit,
            "Minimum torque to apply in case of a angular constraint (N m). Usually this is -mMaxTorqueLimit unless you want a motor that can e.g. push but not pull. Not used when motor is a position motor.")
        .def_rw("max_torque_limit", &MotorSettings::mMaxTorqueLimit,
            "Maximum torque to apply in case of a angular constraint (N m). Not used when motor is a position motor.");
}