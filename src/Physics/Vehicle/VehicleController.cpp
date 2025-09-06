#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleController.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>

void BindVehicleController(nb::module_ &m) {
    nb::class_<VehicleControllerSettings, RefTarget<VehicleControllerSettings>> vehicleControllerSettingsCls(m, "VehicleControllerSettings",
        nb::intrusive_ptr<VehicleControllerSettings>([](VehicleControllerSettings *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Basic settings object for interface that controls acceleration / deceleration of the vehicle");
    vehicleControllerSettingsCls
        .def("save_binary_state", &VehicleControllerSettings::SaveBinaryState, "stream"_a,
            "Saves the contents of the controller settings in binary form to inStream.")
        .def("restore_binary_state", &VehicleControllerSettings::RestoreBinaryState, "stream"_a,
            "Restore the contents of the controller settings in binary form from inStream.")
        .def("construct_controller", &VehicleControllerSettings::ConstructController, "constraint"_a, nb::rv_policy::reference,
            "Create an instance of the vehicle controller class");

    nb::class_<VehicleController, RefTarget<VehicleController>> vehicleControllerCls(m, "VehicleController",
        "Runtime data for interface that controls acceleration / deceleration of the vehicle");
    // vehicleControllerCls
        // .def(nb::init<VehicleConstraint &>(), "constraint"_a,
        //     "Constructor / destructor")
            ;
}