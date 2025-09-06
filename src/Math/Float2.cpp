#include "Common.h"
#include <nanobind/operators.h>
#include <Jolt/Math/Float2.h>

void BindFloat2(nb::module_ &m) {
    nb::class_<Float2>(m, "Float2", "Class that holds 2 floats, used as a storage class mainly.")
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<float, float>(), "x"_a, "y"_a, "Constructor initializing x and y.")
        .def_rw("x", &Float2::x, "x"_a, "x component")
        .def_rw("y", &Float2::y, "y"_a, "y component")

        // Operators
        .def(nb::self == nb::self, "rhs"_a, "Equality comparison.")
        .def(nb::self != nb::self, "rhs"_a, "Inequality comparison.")

        // Python-specific
        .def("__repr__", [](const Float2 &self) {
            return nb::str("Float2({0}, {1})").format(self.x, self.y);
        });
}