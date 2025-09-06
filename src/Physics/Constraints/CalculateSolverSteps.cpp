#include "Common.h"
#include <Jolt/Physics/Constraints/CalculateSolverSteps.h>

void BindCalculateSolverSteps(nb::module_ &m) {
    nb::class_<CalculateSolverSteps> calculateSolverStepsCls(m, "CalculateSolverSteps",
        "Class used to calculate the total number of velocity and position steps");
    calculateSolverStepsCls
        .def(nb::init<const PhysicsSettings &>(), "settings"_a,
            "Constructor")
        // .def("operator()", &CalculateSolverSteps::operator(), "object"_a,           // TODO:
            // "Combine the number of velocity and position steps for this body/constraint with the current values")
        .def("finalize", &CalculateSolverSteps::Finalize,
            "Must be called after all bodies/constraints have been processed")
        .def("get_num_position_steps", &CalculateSolverSteps::GetNumPositionSteps,
            "Get the results of the calculation")
        .def("get_num_velocity_steps", &CalculateSolverSteps::GetNumVelocitySteps);

    nb::class_<DummyCalculateSolverSteps> dummyCalculateSolverStepsCls(m, "DummyCalculateSolverSteps",
            "Dummy class to replace the steps calculator when we don't need the result");
    // dummyCalculateSolverStepsCls
        // .def("do_nothing", &DummyCalculateSolverSteps::operator())
        ;
}