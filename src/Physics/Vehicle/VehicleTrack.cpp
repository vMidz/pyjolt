#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleTrack.h>

void BindVehicleTrack(nb::module_ &m) {
    nb::enum_<ETrackSide>(m, "ETrackSide",
        "On which side of the vehicle the track is located (for steering)")
        .value("LEFT", ETrackSide::Left)
        .value("RIGHT", ETrackSide::Right)
        .value("NUM", ETrackSide::Num);
    nb::class_<VehicleTrackSettings> vehicleTrackSettingsCls(m, "VehicleTrackSettings",
        "Generic properties for tank tracks");
    vehicleTrackSettingsCls
        .def("save_binary_state", &VehicleTrackSettings::SaveBinaryState, "stream"_a,
            "Saves the contents in binary form to inStream.")
        .def("restore_binary_state", &VehicleTrackSettings::RestoreBinaryState, "stream"_a,
            "Restores the contents in binary form to inStream.")
        .def_rw("driven_wheel", &VehicleTrackSettings::mDrivenWheel,
            "Which wheel on the track is connected to the engine")
        .def_rw("wheels", &VehicleTrackSettings::mWheels,
            "Indices of wheels that are inside this track, should include the driven wheel too")
        .def_rw("inertia", &VehicleTrackSettings::mInertia,
            "Moment of inertia (kg m^2) of the track and its wheels as seen on the driven wheel")
        .def_rw("angular_damping", &VehicleTrackSettings::mAngularDamping,
            "Damping factor of track and its wheels: dw/dt = -c * w as seen on the driven wheel")
        .def_rw("max_brake_torque", &VehicleTrackSettings::mMaxBrakeTorque,
            "How much torque (Nm) the brakes can apply on the driven wheel")
        .def_rw("differential_ratio", &VehicleTrackSettings::mDifferentialRatio,
            "Ratio between rotation speed of gear box and driven wheel of track");

    nb::class_<VehicleTrack, VehicleTrackSettings> vehicleTrackCls(m, "VehicleTrack",
        "Runtime data for tank tracks");
    vehicleTrackCls
        .def("save_state", &VehicleTrack::SaveState, "stream"_a,
            "Saving state for replay")
        .def("restore_state", &VehicleTrack::RestoreState, "stream"_a)
        .def_rw("angular_velocity", &VehicleTrack::mAngularVelocity,
            "Angular velocity of the driven wheel, will determine the speed of the entire track");
}