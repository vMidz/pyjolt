#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleEngine.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindVehicleEngine(nb::module_ &m) {
    nb::class_<VehicleEngineSettings> vehicleEngineSettingsCls(m, "VehicleEngineSettings",
        "Generic properties for a vehicle engine");
    vehicleEngineSettingsCls
        .def(nb::init<>(),
            "Constructor")
        .def("save_binary_state", &VehicleEngineSettings::SaveBinaryState, "stream"_a,
            "Saves the contents in binary form to inStream.")
        .def("restore_binary_state", &VehicleEngineSettings::RestoreBinaryState, "stream"_a,
            "Restores the contents in binary form to inStream.")
        .def_rw("max_torque", &VehicleEngineSettings::mMaxTorque,
            "Max amount of torque (Nm) that the engine can deliver")
        .def_rw("min_rpm", &VehicleEngineSettings::mMinRPM,
            "Min amount of revolutions per minute (rpm) the engine can produce without stalling")
        .def_rw("max_rpm", &VehicleEngineSettings::mMaxRPM,
            "Max amount of revolutions per minute (rpm) the engine can generate")
        .def_rw("normalized_torque", &VehicleEngineSettings::mNormalizedTorque,
            "Y-axis: Curve that describes a ratio of the max torque the engine can produce (0 = 0, 1 = mMaxTorque). X-axis: the fraction of the RPM of the engine (0 = mMinRPM, 1 = mMaxRPM)")
        .def_rw("inertia", &VehicleEngineSettings::mInertia,
            "Moment of inertia (kg m^2) of the engine")
        .def_rw("angular_damping", &VehicleEngineSettings::mAngularDamping,
            "Angular damping factor of the wheel: dw/dt = -c * w");

    nb::class_<VehicleEngine, VehicleEngineSettings> vehicleEngineCls(m, "VehicleEngine",
        "Runtime data for engine");
    vehicleEngineCls
        .def("clamp_rpm", &VehicleEngine::ClampRPM,
            "Clamp the RPM between min and max RPM")
        .def("get_current_rpm", &VehicleEngine::GetCurrentRPM,
            "Current rotation speed of engine in rounds per minute")
        .def("set_current_rpm", &VehicleEngine::SetCurrentRPM, "rpm"_a,
            "Update rotation speed of engine in rounds per minute")
        .def("get_angular_velocity", &VehicleEngine::GetAngularVelocity,
            "Get current angular velocity of the engine in radians / second")
        .def("get_torque", &VehicleEngine::GetTorque, "acceleration"_a,
            "Get the amount of torque (N m) that the engine can supply.\n"
            "Args:\n"
            "    acceleration (float): How much the gas pedal is pressed [0, 1].")
        .def("apply_torque", &VehicleEngine::ApplyTorque, "torque"_a, "delta_time"_a,
            "Apply a torque to the engine rotation speed.\n"
            "Args:\n"
            "    torque (float): Torque in N m.\n"
            "    delta_time (float): Delta time in seconds.")
        .def("apply_damping", &VehicleEngine::ApplyDamping, "delta_time"_a,
            "Update the engine RPM for damping.\n"
            "Args:\n"
            "    delta_time (float): Delta time in seconds.")
        .def("convert_rpm_to_angle", &VehicleEngine::ConvertRPMToAngle, "rpm"_a)
        .def("draw_rpm", &VehicleEngine::DrawRPM, "renderer"_a, "position"_a, "forward"_a, "up"_a, "size"_a, "shift_down_rpm"_a, "shift_up_rpm"_a,
            "Debug draw a RPM meter")
        .def("allow_sleep", &VehicleEngine::AllowSleep,
            "If the engine is idle we allow the vehicle to sleep")
        .def("save_state", &VehicleEngine::SaveState, "stream"_a,
            "Saving state for replay")
        .def("restore_state", &VehicleEngine::RestoreState, "stream"_a)
        .def_ro_static("C_ANGULAR_VELOCITY_TO_RPM", &VehicleEngine::cAngularVelocityToRPM,
            "Multiply an angular velocity (rad/s) with this value to get rounds per minute (RPM)");
}