#include "Common.h"

void BindFloat4(nb::module_ &m) {
    nb::class_<Float4>(m, "Float4", "Class that holds 4 float values. Convert to Vec4 to perform calculations.")
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<float, float, float, float>(), "x"_a, "y"_a, "z"_a, "w"_a, "Constructor initializing x, y, z, and w.")

        .def_rw("x", &Float4::x, "x"_a, "x component")
        .def_rw("y", &Float4::y, "y"_a, "y component")
        .def_rw("z", &Float4::z, "z"_a, "z component")
        .def_rw("w", &Float4::w, "w"_a, "w component")

        .def("__getitem__", [](const Float4 &v, int i) {
            if (i < 0 || i >= 4)
                throw nb::index_error("Index out of bounds (must be 0, 1, 2, or 3)");

            return v[i];
        }, "i"_a, "Get component by index (0=x, 1=y, 2=z, 3=w).")

        // Python-specific
        .def("__repr__", [](const Float4 &self) {
            return nb::str("Float2({0}, {1}, {2}, {3})").format(self.x, self.y, self.z, self.w);
        });
}