#include "Common.h"
#include <Jolt/Math/Float3.h>
#include <nanobind/operators.h>

void BindFloat3(nb::module_ &m) {
    nb::class_<Float3>(m, "Float3", "Class that holds 3 floats. Used as a storage class. Convert to Vec3 for calculations.")
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<float, float, float>(), "x"_a, "y"_a, "z"_a, "Constructor initializing x, y, and z.")
        .def_rw("x", &Float3::x, "x"_a, "x component")
        .def_rw("y", &Float3::y, "y"_a, "y component")
        .def_rw("z", &Float3::z, "z"_a, "z component")

        // Operators
        .def("__getitem__", [](const Float3 &v, int i) {
            if (i < 0 || i >= 3)
                throw nb::index_error("Index out of bounds (must be 0, 1, or 2)");

            return v[i];
        }, "i"_a, "Get component by index (0=x, 1=y, 2=z).")
        .def(nb::self == nb::self, "rhs"_a, "Equality operator.")
        .def(nb::self != nb::self, "rhs"_a, "Inequality operator.")

        // Python-specific
        .def("__hash__", [](const Float3 &v) {
            return nb::hash(nb::make_tuple(v.x, v.y, v.z));
        })
        .def("__repr__", [](const Float3 &self) {
            return nb::str("Float2({0}, {1}, {2})").format(self.x, self.y, self.z);
        });
}
