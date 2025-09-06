#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleDifferential.h>

void BindVehicleDifferential(nb::module_ &m) {
    nb::class_<VehicleDifferentialSettings>(m, "VehicleDifferentialSettings")
        .def("save_binary_state", &VehicleDifferentialSettings::SaveBinaryState, "stream"_a, "Saves the contents in binary form to inStream.")
        .def("restore_binary_state", &VehicleDifferentialSettings::RestoreBinaryState, "stream"_a, "Restores the contents in binary form to inStream.")
        .def("calculate_torque_ratio", [](VehicleDifferentialSettings &self, float inLeftAngularVelocity, float inRightAngularVelocity) {
            float outLeftTorqueFraction;
            float outRightTorqueFraction;
            self.CalculateTorqueRatio(inLeftAngularVelocity, inRightAngularVelocity, outLeftTorqueFraction, outRightTorqueFraction);
            return nb::make_tuple(outLeftTorqueFraction, outRightTorqueFraction);
        }, "left_angular_velocity"_a, "right_angular_velocity"_a,
            "Calculate the torque ratio between left and right wheel.\n"
            "Args:\n"
            "    left_angular_velocity (float): Angular velocity of left wheel (rad / s).\n"
            "    right_angular_velocity (float): Angular velocity of right wheel (rad / s).\n"
            "    left_torque_fraction (float): Fraction of torque that should go to the left wheel.\n"
            "    right_torque_fraction (float): Fraction of torque that should go to the right wheel.")

        .def_rw("left_wheel", &VehicleDifferentialSettings::mLeftWheel,
            "Index (in mWheels) that represents the left wheel of this differential (can be -1 to indicate no wheel)")
        .def_rw("right_wheel", &VehicleDifferentialSettings::mRightWheel,
            "Index (in mWheels) that represents the right wheel of this differential (can be -1 to indicate no wheel)")
        .def_rw("differential_ratio", &VehicleDifferentialSettings::mDifferentialRatio,
            "Ratio between rotation speed of gear box and wheels")
        .def_rw("left_right_split", &VehicleDifferentialSettings::mLeftRightSplit,
            "Defines how the engine torque is split across the left and right wheel (0 = left, 0.5 = center, 1 = right)")
        .def_rw("limited_slip_ratio", &VehicleDifferentialSettings::mLimitedSlipRatio,
            "Ratio max / min wheel speed. When this ratio is exceeded, all torque gets distributed to the slowest moving wheel. This allows implementing a limited slip differential. Set to FLT_MAX for an open differential. Value should be > 1.")
        .def_rw("engine_torque_ratio", &VehicleDifferentialSettings::mEngineTorqueRatio,
            "How much of the engines torque is applied to this differential (0 = none, 1 = full), make sure the sum of all differentials is 1.");
}