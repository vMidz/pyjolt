#include "Common.h"
#include <nanobind/ndarray.h>
#include <nanobind/operators.h>

using NumpyDouble3 = nb::ndarray<nb::numpy, double, nb::shape<3>, nb::device::cpu, nb::c_contig>;

Double3 numpy_to_double3(const NumpyDouble3 &numpy_arr) {
    const double *data = numpy_arr.data();
    return Double3(data[0], data[1], data[2]);
}

NumpyDouble3 double3_to_numpy(const Double3 &self) {
    double *data = new double[3];
    nb::capsule owner(data, [](void *p) noexcept {
        delete[] (double *)p;
    });

    data[0] = self.x;
    data[1] = self.y;
    data[2] = self.z;

    return NumpyDouble3(data, {3}, owner);
}

void BindDouble3(nb::module_ &m) {
    nb::class_<Double3>(m, "Double3", "Class that holds 3 doubles. Used as a storage class. Convert to DVec3 for calculations.")
        // Constructors
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<const Double3 &>(), "rhs"_a)
        .def(nb::init<double, double, double>(), "x"_a, "y"_a, "z"_a)

        // Operators
        .def("__getitem__", [](const Double3 &self, int i) {
            if (i < 0 || i >= 3)
                throw nb::index_error("Index out of range");
            return self[i];
        }, "index"_a)
        .def(nb::self == nb::self, "rhs"_a)
        .def(nb::self != nb::self, "rhs"_a)

        .def_rw("x", &Double3::x, "x"_a)
        .def_rw("y", &Double3::y, "y"_a)
        .def_rw("z", &Double3::z, "z"_a)

        // Python-specific
        .def_static("from_numpy", &numpy_to_double3, "numpy_array"_a, "Convert Numpy array to Double3, data is cloned")
        .def("to_numpy", [](const Double3 &self) {
            return double3_to_numpy(self);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"), "Convert Double3 to NumPy array, data is cloned")

        .def("__repr__", [](const Double3 &self) {
            return nb::str("Double3({0}, {1}, {2})").format(self.x, self.y, self.z);
        });
}
