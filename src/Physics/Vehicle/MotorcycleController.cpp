#include "Common.h"
#include <Jolt/Physics/Vehicle/MotorcycleController.h>

void BindMotorcycleController(nb::module_ &m) {
    nb::class_<MotorcycleControllerSettings, WheeledVehicleControllerSettings> motorcycleControllerSettingsCls(m, "MotorcycleControllerSettings",
        "Settings of a two wheeled motorcycle (adds a spring to balance the motorcycle)\n"
        "Note: The motor cycle controller is still in development and may need a lot of tweaks/hacks to work properly!");
    motorcycleControllerSettingsCls
        .def("construct_controller", &MotorcycleControllerSettings::ConstructController, "constraint"_a, nb::rv_policy::reference)
        .def("save_binary_state", &MotorcycleControllerSettings::SaveBinaryState, "stream"_a)
        .def("restore_binary_state", &MotorcycleControllerSettings::RestoreBinaryState, "stream"_a)
        .def_rw("max_lean_angle", &MotorcycleControllerSettings::mMaxLeanAngle,
            "How far we're willing to make the bike lean over in turns (in radians)")
        .def_rw("lean_spring_constant", &MotorcycleControllerSettings::mLeanSpringConstant,
            "Spring constant for the lean spring")
        .def_rw("lean_spring_damping", &MotorcycleControllerSettings::mLeanSpringDamping,
            "Spring damping constant for the lean spring")
        .def_rw("lean_spring_integration_coefficient", &MotorcycleControllerSettings::mLeanSpringIntegrationCoefficient,
            "The lean spring applies an additional force equal to this coefficient * Integral(delta angle, 0, t), this effectively makes the lean spring a PID controller")
        .def_rw("lean_spring_integration_coefficient_decay", &MotorcycleControllerSettings::mLeanSpringIntegrationCoefficientDecay,
            "How much to decay the angle integral when the wheels are not touching the floor: new_value = e^(-decay * t) * initial_value")
        .def_rw("lean_smoothing_factor", &MotorcycleControllerSettings::mLeanSmoothingFactor,
            "How much to smooth the lean angle (0 = no smoothing, 1 = lean angle never changes)\n"
            "Note that this is frame rate dependent because the formula is: smoothing_factor * previous + (1 - smoothing_factor) * current");

    nb::class_<MotorcycleController, WheeledVehicleController> motorcycleControllerCls(m, "MotorcycleController",
        "Runtime controller class");
    motorcycleControllerCls
        .def(nb::init<const MotorcycleControllerSettings &, VehicleConstraint &>(), "settings"_a, "constraint"_a,
            "Constructor")
        .def("get_wheel_base", &MotorcycleController::GetWheelBase,
            "Get the distance between the front and back wheels")
        .def("enable_lean_controller", &MotorcycleController::EnableLeanController, "enable"_a,
            "Enable or disable the lean spring. This allows you to temporarily disable the lean spring to allow the motorcycle to fall over.")
        .def("is_lean_controller_enabled", &MotorcycleController::IsLeanControllerEnabled,
            "Check if the lean spring is enabled.")
        .def("enable_lean_steering_limit", &MotorcycleController::EnableLeanSteeringLimit, "enable"_a,
            "Enable or disable the lean steering limit. When enabled (default) the steering angle is limited based on the vehicle speed to prevent steering that would cause an inertial force that causes the motorcycle to topple over.")
        .def("is_lean_steering_limit_enabled", &MotorcycleController::IsLeanSteeringLimitEnabled)
        .def("set_lean_spring_constant", &MotorcycleController::SetLeanSpringConstant, "constant"_a,
            "Spring constant for the lean spring")
        .def("get_lean_spring_constant", &MotorcycleController::GetLeanSpringConstant)
        .def("set_lean_spring_damping", &MotorcycleController::SetLeanSpringDamping, "damping"_a,
            "Spring damping constant for the lean spring")
        .def("get_lean_spring_damping", &MotorcycleController::GetLeanSpringDamping)
        .def("set_lean_spring_integration_coefficient", &MotorcycleController::SetLeanSpringIntegrationCoefficient, "coefficient"_a,
            "The lean spring applies an additional force equal to this coefficient * Integral(delta angle, 0, t), this effectively makes the lean spring a PID controller")
        .def("get_lean_spring_integration_coefficient", &MotorcycleController::GetLeanSpringIntegrationCoefficient)
        .def("set_lean_spring_integration_coefficient_decay", &MotorcycleController::SetLeanSpringIntegrationCoefficientDecay, "decay"_a,
            "How much to decay the angle integral when the wheels are not touching the floor: new_value = e^(-decay * t) * initial_value")
        .def("get_lean_spring_integration_coefficient_decay", &MotorcycleController::GetLeanSpringIntegrationCoefficientDecay)
        .def("set_lean_smoothing_factor", &MotorcycleController::SetLeanSmoothingFactor, "factor"_a,
            "How much to smooth the lean angle (0 = no smoothing, 1 = lean angle never changes)\n"
            "Note that this is frame rate dependent because the formula is: smoothing_factor * previous + (1 - smoothing_factor) * current")
        .def("get_lean_smoothing_factor", &MotorcycleController::GetLeanSmoothingFactor);
}