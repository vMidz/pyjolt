#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleAntiRollBar.h>

void BindVehicleAntiRollBar(nb::module_ &m) {
    nb::class_<VehicleAntiRollBar> vehicleAntiRollBarCls(m, "VehicleAntiRollBar",
        "An anti rollbar is a stiff spring that connects two wheels to reduce the amount of roll the vehicle makes in sharp corners\n"
        "See: https://en.wikipedia.org/wiki/Anti-roll_bar");
    vehicleAntiRollBarCls
        .def("save_binary_state", &VehicleAntiRollBar::SaveBinaryState, "stream"_a,
            "Saves the contents in binary form to inStream.")
        .def("restore_binary_state", &VehicleAntiRollBar::RestoreBinaryState, "stream"_a,
            "Restores the contents in binary form to inStream.")
        .def_rw("left_wheel", &VehicleAntiRollBar::mLeftWheel,
            "Index (in mWheels) that represents the left wheel of this anti-rollbar")
        .def_rw("right_wheel", &VehicleAntiRollBar::mRightWheel,
            "Index (in mWheels) that represents the right wheel of this anti-rollbar")
        .def_rw("stiffness", &VehicleAntiRollBar::mStiffness,
            "Stiffness (spring constant in N/m) of anti rollbar, can be 0 to disable the anti-rollbar");
}