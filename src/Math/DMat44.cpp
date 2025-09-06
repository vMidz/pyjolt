#include "Common.h"
#include <Jolt/Math/DMat44.h>

#include "BindingUtility/Tuple.h"
#include <nanobind/ndarray.h>
#include <nanobind/operators.h>

// Column-major
using NumpyDMat44 = nb::ndarray<nb::numpy, double, nb::shape<4, 4>, nb::device::cpu, nb::c_contig>;

// Helper functions for conversion between NumPy arrays and DMat44
DMat44 numpy_to_dmat44(const NumpyDMat44 &numpy_arr) {
    // Extract data from NumPy array
    const double *data = numpy_arr.data();
    Vec4 col0(data[0], data[1], data[2], data[3]);
    Vec4 col1(data[4], data[5], data[6], data[7]);
    Vec4 col2(data[8], data[9], data[10], data[11]);
    DVec3 col3(data[12], data[13], data[14]); // data[15] is assumed to be 1.0

    return DMat44(col0, col1, col2, col3);
}

NumpyDMat44 dmat44_to_numpy(const DMat44 &self) {
    // Allocate memory
    double *data = new double[4 * 4];
    nb::capsule owner(data, [](void *p) noexcept {
        delete[] (double *)p;
    });

    // Get columns from matrix
    Vec4 col0 = self.GetColumn4(0);
    Vec4 col1 = self.GetColumn4(1);
    Vec4 col2 = self.GetColumn4(2);
    DVec3 col3 = self.GetTranslation();

    // Fill array in column-major order
    data[0] = col0.GetX();
    data[1] = col0.GetY();
    data[2] = col0.GetZ();
    data[3] = col0.GetW();

    data[4] = col1.GetX();
    data[5] = col1.GetY();
    data[6] = col1.GetZ();
    data[7] = col1.GetW();

    data[8] = col2.GetX();
    data[9] = col2.GetY();
    data[10] = col2.GetZ();
    data[11] = col2.GetW();

    data[12] = col3.GetX();
    data[13] = col3.GetY();
    data[14] = col3.GetZ();
    data[15] = 1.0; // Last element is always 1.0

    return NumpyDMat44(data, {4, 4}, owner);
}

void BindDMat44(nb::module_ &m) {
    nb::class_<DMat44>(m, "DMat44",
            "Holds a 4x4 matrix of floats with the last column consisting of doubles, Translation column, 4th element is assumed to be 1")
        // Constructors
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<Vec4, Vec4, Vec4, DVec3>(), "column_1"_a, "column_2"_a, "column_3"_a, "column_4"_a)
        .def(nb::init<DMat44>(), "matrix"_a)
        .def(nb::init<Mat44>(), "matrix"_a)
        .def(nb::init<Mat44, DVec3>(), "rotation"_a, "translation"_a)

        // Static methods
        .def_static("zero", &DMat44::sZero, "Zero matrix")
        .def_static("identity", &DMat44::sIdentity, "Identity matrix")
        .def_static("rotation", &DMat44::sRotation, "quaternion"_a, "Rotate from quaternion")
        .def_static("create_translation_matrix", &DMat44::sTranslation, "vector"_a, "Get matrix that translates")
        .def_static("rotation_translation", &DMat44::sRotationTranslation, "rotation"_a, "translation"_a,
            "Get matrix that rotates and translates")
        .def_static("inverse_rotation_translation", &DMat44::sInverseRotationTranslation, "rotation"_a, "translation"_a,
            "Get inverse matrix of rotation_translation")
        .def_static("scale", &DMat44::sScale, "vector"_a, "Get matrix that scales (produces a matrix with (vector, 1) on its diagonal)")

        .def("to_mat44", &DMat44::ToMat44, "Convert to Mat44 rounding to nearest")
        .def("is_close", &DMat44::IsClose, "matrix"_a, "max_dist_sq"_a = 1.0e-12f, "Test if two matrices are close")
        .def("multiply_3x3", nb::overload_cast<Vec3Arg>(&DMat44::Multiply3x3, nb::const_), "vector"_a,
            "Multiply vector by only 3x3 part of the matrix")
        .def("multiply_3x3", nb::overload_cast<DVec3Arg>(&DMat44::Multiply3x3, nb::const_), "vector"_a,
            "Multiply vector by only 3x3 part of the matrix")
        .def("multiply_3x3_transposed", &DMat44::Multiply3x3Transposed, "vector"_a,
            "Multiply vector by only 3x3 part of the transpose of the matrix (\\f$result = this^T : inV\\f$)")

        .def("pre_scaled", &DMat44::PreScaled, "scale"_a, "Scale a matrix: result = this * Mat44().scale(scale)")
        .def("post_scaled", &DMat44::PostScaled, "scale"_a, "Scale a matrix: result = Mat44().scale(scale) * this")
        .def("pre_translated", nb::overload_cast<Vec3Arg>(&DMat44::PreTranslated, nb::const_), "translation"_a,
            "Pre multiply by translation matrix: result = this * Mat44().translation(translation)")
        .def("pre_translated", nb::overload_cast<DVec3Arg>(&DMat44::PreTranslated, nb::const_), "translation"_a,
            "Pre multiply by translation matrix: result = this * Mat44().translation(translation)")
        .def("post_translated", nb::overload_cast<Vec3Arg>(&DMat44::PostTranslated, nb::const_), "translation"_a,
            "Post multiply by translation matrix: result = Mat44().translation(translation) * this (i.e. add translation to the 4-th column)")
        .def("post_translated", nb::overload_cast<DVec3Arg>(&DMat44::PostTranslated, nb::const_), "translation"_a,
            "Post multiply by translation matrix: result = Mat44().translation(translation) * this (i.e. add translation to the 4-th column)")

        .def_prop_rw("axis_x", &DMat44::GetAxisX, &DMat44::SetAxisX, "vector"_a, "Get/set axis X")
        .def_prop_rw("axis_y", &DMat44::GetAxisY, &DMat44::SetAxisY, "vector"_a, "Get/set axis Y")
        .def_prop_rw("axis_z", &DMat44::GetAxisZ, &DMat44::SetAxisZ, "vector"_a, "Get/set axis Z")

        .def_prop_rw("translation", &DMat44::GetTranslation, &DMat44::SetTranslation, "vector"_a, "Get/set translation")

        .def("get_column3", &DMat44::GetColumn3, "column"_a) // Can't use def_prop_rw since they take parameters
        .def("set_column3", &DMat44::SetColumn3, "column"_a, "vector"_a)

        .def("get_column4", &DMat44::GetColumn4, "column"_a)
        .def("set_column4", &DMat44::SetColumn4, "column"_a, "vector"_a)

        .def("transposed_3x3", &DMat44::Transposed3x3, "Transpose 3x3 subpart of matrix")
        .def("inversed", &DMat44::Inversed, "Inverse 4x4 matrix")
        .def("inversed_rotation_translation", &DMat44::InversedRotationTranslation,
            "Inverse 4x4 matrix when it only contains rotation and translation")
        .def_prop_rw("rotation", &DMat44::GetRotation, &DMat44::SetRotation, "rotation"_a,
            "Get/set rotation part only (note: retains the first 3 values from the bottom row)")
        .def("get_quaternion", &DMat44::GetQuaternion, "Convert to quaternion")
        .def("get_direction_preserving_matrix", &DMat44::GetDirectionPreservingMatrix,
            "Get matrix that transforms a direction with the same transform as this matrix (length is not preserved)")
        .def("decompose", [](const DMat44 &self) {
            Vec3 outScale;
            return CreateTuple(self.Decompose(outScale), outScale);
        }, "Works identical to Mat44::Decompose\n"
           "Returns:\n"
           "    tuple:\n"
           "    - DMat44: Rotation & translation part\n"
           "    - Vec3: Scale part")

        // Operators
        .def(nb::self == nb::self, "rhs"_a)
        .def(nb::self != nb::self, "rhs"_a)
        .def(nb::self * Mat44(), "rhs"_a, "Multiply matrix by matrix")
        .def(nb::self * DMat44(), "rhs"_a, "Multiply matrix by double matrix")
        .def(nb::self * Vec3(), "rhs"_a, "Multiply matrix by vector 3")
        .def(nb::self * DVec3(), "rhs"_a, "Multiply vector by double vector 3")

        // Python-specific
        .def_static("from_numpy", &numpy_to_dmat44, "numpy_array"_a, "Convert a 4x4 NumPy array to DMat44,data is cloned")
        .def("to_numpy", [](const DMat44 &self) {
            return dmat44_to_numpy(self);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"), "Convert DMat44 to a 4x4 NumPy array, data is cloned")

        .def("__repr__", [](const DMat44 &self) {
            const auto col0 = self.GetColumn4(0);
            const auto col1 = self.GetColumn4(1);
            const auto col2 = self.GetColumn4(2);
            const auto translation = self.GetTranslation();

            return nb::str("DMat44([[{0}, {1}, {2}, {3}],\n"
                           "       [{4}, {5}, {6}, {7}],\n"
                           "       [{8}, {9}, {10}, {11}],\n"
                           "       [{12}, {13}, {14}, {15}]])")
                .format(col0.GetX(), col1.GetX(), col2.GetX(), translation.GetX(),
                    col0.GetY(), col1.GetY(), col2.GetY(), translation.GetY(),
                    col0.GetZ(), col1.GetZ(), col2.GetZ(), translation.GetZ(),
                    col0.GetW(), col1.GetW(), col2.GetW(), 1.0);
        });
}
