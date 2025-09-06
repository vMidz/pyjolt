#include "Common.h"
#include <Jolt/Math/Mat44.h>
#include "BindingUtility/Tuple.h"
#include <nanobind/ndarray.h>
#include <nanobind/operators.h>

// Column-major
using NumpyMat44 = nb::ndarray<nb::numpy, float, nb::shape<4, 4>, nb::device::cpu, nb::c_contig>;

// Helper functions for conversion between NumPy arrays and Mat44
Mat44 numpy_to_mat44(const NumpyMat44 &numpy_arr) {
    // Extract data from NumPy array
    const float *data = numpy_arr.data();
    Vec4 col1(data[0], data[1], data[2], data[3]);
    Vec4 col2(data[4], data[5], data[6], data[7]);
    Vec4 col3(data[8], data[9], data[10], data[11]);
    Vec4 col4(data[12], data[13], data[14], data[15]);

    return Mat44(col1, col2, col3, col4);
}

NumpyMat44 mat44_to_numpy(const Mat44 &self) {
    // Allocate memory
    float *data = new float[4 * 4];
    nb::capsule owner(data, [](void *p) noexcept {
        delete[] (float *)p;
    });

    memcpy(data, &const_cast<Mat44 &>(self)(0, 0), sizeof(Mat44));

    // for (size_t c = 0; c < 4; ++c)
    //     for (size_t r = 0; r < 4; ++r)
    //         data[r + (4 * c)] = self(r, c);

    return NumpyMat44(data, {4, 4}, owner);
}

void BindMat44(nb::module_ &m) {
    nb::class_<Mat44>(m, "Mat44", "Holds a 4x4 matrix of floats, but supports also operations on the 3x3 upper left part of the matrix.")
        .def(nb::init<>(), "Default constructor")
        .def(nb::init<Vec4, Vec4, Vec4, Vec4>(),
            "col_1"_a, "col_2"_a, "col_3"_a, "col_4"_a, "Constructor from four Vec4 columns")
        .def(nb::init<Vec4, Vec4, Vec4, Vec3>(),
            "col_1"_a, "col_2"_a, "col_3"_a, "col_4"_a, "Constructor from three Vec4 columns and one Vec3 column")
        .def(nb::init<const Mat44 &>(), "matrix"_a)

        .def_static("zero", &Mat44::sZero, "Zero matrix")
        .def_static("identity", &Mat44::sIdentity, "identity matrix")
        .def_static("nan", &Mat44::sNaN, "Matrix filled with NaN's")
        .def_static("load_float4x4", &Mat44::sLoadFloat4x4, "vector"_a, "Load 16 floats from memory")
        .def_static("load_float4x4_aligned", &Mat44::sLoadFloat4x4Aligned, "vector"_a, "Load 16 floats from memory, 16 bytes aligned")
        .def_static("rotation_x", &Mat44::sRotationX, "x_angle_radians"_a, "Rotate around X, Y or Z axis (angle in radians)")
        .def_static("rotation_y", &Mat44::sRotationY, "y_angle_radians"_a)
        .def_static("rotation_z", &Mat44::sRotationZ, "z_angle_radians"_a)
        .def_static("create_rotation_matrix", nb::overload_cast<QuatArg>(&Mat44::sRotation),
            "quaternion"_a, "Rotate from quaternion")
        .def_static("create_rotation_matrix", nb::overload_cast<Vec3Arg, float>(&Mat44::sRotation),
            "axis"_a, "angle"_a, "Rotate around arbitrary axis")
        .def_static("create_translation_matrix", &Mat44::sTranslation, "translation"_a, "Get matrix that translates")
        .def_static("create_rotation_translation_matrix", &Mat44::sRotationTranslation, "rotation"_a, "translation"_a, "Create a matrix that rotates and translates")
        .def_static("create_inverse_rotation_translation", &Mat44::sInverseRotationTranslation,
            "rotation"_a, "translation"_a, "Get inverse matrix of rotation_translation")
        .def_static("create_scale_matrix", nb::overload_cast<Vec3Arg>(&Mat44::sScale),
            "scale"_a,
            "Get matrix that scales uniformly")
        .def_static("create_scale_matrix", nb::overload_cast<float>(&Mat44::sScale),
            "scale"_a,
            "Get matrix that scales (produces a matrix with (inV, 1) on its diagonal)")
        .def_static("outer_product", &Mat44::sOuterProduct,
            "vector"_a, "vector2"_a, "Get outer product of inV and inV2 (equivalent to \\f$inV1 \\otimes inV2\\f$)")
        .def_static("cross_product", &Mat44::sCrossProduct,
            "vector"_a, "Get matrix that represents a cross product \\f$A \\times B = \\text{sCrossProduct}(A) : B\\f$")
        .def_static("quat_left_multiply", &Mat44::sQuatLeftMultiply,
            "quaternion"_a, "Returns matrix ML so that \\f$ML(q) : p = q : p\\f$ (where p and q are quaternions)")
        .def_static("quat_right_multiply", &Mat44::sQuatRightMultiply,
            "quaternion"_a, "Returns matrix MR so that \\f$MR(q) : p = p : q\\f$ (where p and q are quaternions)")
        .def_static("look_at", &Mat44::sLookAt, "position"_a, "target"_a, "up"_a,
            "Returns a look at matrix that transforms from world space to view space.\n"
            "Args:\n"
            "    pos (Vec3): Position of the camera.\n"
            "    target (Vec3): Target of the camera.\n"
            "    up (Vec3): Up vector.")
        .def_static("perspective", &Mat44::sPerspective, "fov_y"_a, "aspect"_a, "near"_a, "far"_a, "Returns a right-handed perspective projection matrix")

        .def("is_close", &Mat44::IsClose, "other"_a, "max_dist_sq"_a = 1.0e-12f, "Test if two matrices are close")
        .def("multiply_3x3", nb::overload_cast<Mat44Arg>(&Mat44::Multiply3x3, nb::const_),
            "matrix"_a, "Multiply vector by only 3x3 part of the matrix")
        .def("multiply_3x3", nb::overload_cast<Vec3Arg>(&Mat44::Multiply3x3, nb::const_),
            "vector"_a, "Multiply 3x3 matrix by 3x3 matrix")
        .def("multiply3x3_transposed", &Mat44::Multiply3x3Transposed,
            "vector"_a, "Multiply vector by only 3x3 part of the transpose of the matrix (\\f$result = this^T : inV\\f$)")
        .def("multiply3x3_left_transposed", &Mat44::Multiply3x3LeftTransposed,
            "matrix"_a, "Multiply transpose of 3x3 matrix by 3x3 matrix (\\f$result = this^T : inM\\f$)")
        .def("multiply3x3_right_transposed", &Mat44::Multiply3x3RightTransposed,
            "matrix"_a, "Multiply 3x3 matrix by the transpose of a 3x3 matrix (\\f$result = this : inM^T\\f$")

        .def_prop_rw("axis_x", &Mat44::GetAxisX, &Mat44::SetAxisX, "x_axis"_a, "Get/set X axis")
        .def_prop_rw("axis_y", &Mat44::GetAxisY, &Mat44::SetAxisY, "y_axis"_a, "Get/set Y axis")
        .def_prop_rw("axis_z", &Mat44::GetAxisZ, &Mat44::SetAxisZ, "z_axis"_a, "Get/set Z axis")
        .def_prop_rw("translation", &Mat44::GetTranslation, &Mat44::SetTranslation, "vector"_a, "Get/set translation")

        .def_prop_rw("diagonal_3", &Mat44::GetDiagonal3, &Mat44::SetDiagonal3, "vector"_a, "Get/set diagonal of 3x3 part")
        .def_prop_rw("diagonal_4", &Mat44::GetDiagonal4, &Mat44::SetDiagonal4, "vector"_a, "Get/set diagonal of 4x4 part")

        .def("get_column3", &Mat44::GetColumn3, "column"_a) // Can't use def_prop_rw since they take parameters
        .def("set_column3", &Mat44::SetColumn3, "column"_a, "vector"_a)

        .def("get_column4", &Mat44::GetColumn4, "column"_a)
        .def("set_column4", &Mat44::SetColumn4, "column"_a, "vector"_a)

        .def("store_float4x4", &Mat44::StoreFloat4x4, "out"_a, "Store matrix to memory")
        .def("transposed", &Mat44::Transposed, "Transpose matrix")
        .def("transposed_3x3", &Mat44::Transposed3x3, "Transpose 3x3 subpart of matrix")
        .def("inversed", &Mat44::Inversed, "Inverse 4x4 matrix")
        .def("inversed_rotation_translation", &Mat44::InversedRotationTranslation, "Inverse 4x4 matrix when it only contains rotation and translation")
        .def("get_determinant_3x3", &Mat44::GetDeterminant3x3, "Get the determinant of a 3x3 matrix")
        .def("adjointed_3x3", &Mat44::Adjointed3x3, "Get the adjoint of a 3x3 matrix")
        .def("inversed_3x3", &Mat44::Inversed3x3, "Inverse 3x3 matrix")
        .def("set_inversed_3x3", &Mat44::SetInversed3x3,
            "matrix"_a, "this = inM.Inversed3x3(), returns false if the matrix is singular in which case *this is unchanged")
        .def_prop_rw("rotation", &Mat44::GetRotation, &Mat44::SetRotation,
            "rotation"_a, "Get/set rotation part only (note: retains the first 3 values from the bottom row)")
        .def("get_rotation_safe", &Mat44::GetRotationSafe, "Get rotation part only (note: also clears the bottom row)")
        .def("get_quaternion", &Mat44::GetQuaternion, "Convert to quaternion")
        .def("get_direction_preserving_matrix", &Mat44::GetDirectionPreservingMatrix,
            "Get matrix that transforms a direction with the same transform as this matrix (length is not preserved)")
        .def("pre_translated", &Mat44::PreTranslated,
            "translation"_a, "Pre multiply by translation matrix: result = this * Mat44.Translation(translation)")
        .def("post_translated", &Mat44::PostTranslated,
            "translation"_a, "Post multiply by translation matrix: result = Mat44.Translation(translation) * this (i.e. add translation to the 4-th column)")
        .def("pre_scaled", &Mat44::PreScaled, "scale"_a, "Scale a matrix: result = this * Mat44.scale(scale)")
        .def("post_scaled", &Mat44::PostScaled, "scale"_a, "Scale a matrix: result = Mat44.scale(inScscaleale) * this")
        .def("decompose", [](const Mat44 &self) {
            Vec3 scale;
            return CreateTuple(self.Decompose(scale), scale);
        }, "Decompose a matrix into a rotation & translation part and into a scale part so that:\n"
           "this = return_value * Mat44::sScale(outScale).\n"
           "This equation only holds when the matrix is orthogonal, if it is not the returned matrix\n"
           "will be made orthogonal using the modified Gram-Schmidt algorithm (see: https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process)\n"
           "Returns\n"
           "    tuple:\n"
           "    - Mat44: Rotation & translation part\n"
           "    - Vec3: Scale part")

        .def("to_mat44", &Mat44::ToMat44, "In single precision mode just return the matrix itself")

        // Operators
        .def("__getitem__", [](const Mat44 &self, nb::typed<nb::tuple, int, int> &tuple) {
            auto first = nb::cast<int>(tuple[0]);
            auto second = nb::cast<int>(tuple[1]);
            if (second >= 4 || first >= 4)
                throw nb::index_error("Matrix index out of range");
            return self(first, second);
        }, "index"_a, "Get float component by element index")
        .def("__setitem__", [](Mat44 &self, nb::typed<nb::tuple, int, int> &tuple, float value) {
            auto first = nb::cast<int>(tuple[0]);
            auto second = nb::cast<int>(tuple[1]);
            if (first >= 4 || second >= 4)
                throw nb::index_error("Matrix index out of range");
            self(first, second) = value;
        }, "index"_a, "value"_a, "Set float component by element index")

        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a, "Comparison")

        .def(nb::self * nb::self, "rhs"_a, "Multiply matrix by matrix")
        .def(nb::self * Vec3(), "rhs"_a, "Multiply vector by matrix")
        .def(nb::self * Vec4(), "rhs"_a, "Multiply vector by matrix")
        .def(nb::self * float(), "rhs"_a, "Multiply matrix with float")
        .def(float() * nb::self, "rhs"_a, "Multiply matrix with float")
        .def(nb::self *= float(), "rhs"_a, "Multiply matrix with float")

        .def(nb::self + nb::self, "rhs"_a, "Per element addition of matrix")
        .def(nb::self += nb::self, "rhs"_a, "Per element addition of matrix")
        .def(nb::self - nb::self, "rhs"_a, "Per element subtraction of matrix")
        .def(-nb::self, "Negate")

        // Python-specific
        .def_static("orthographic", [](float inLeft, float inRight, float inBottom, float inTop, float inNear, float inFar){
            float r_l = inRight - inLeft;
            float t_b = inTop - inBottom;
            float f_n = inFar - inNear;

            float tx = -(inRight + inLeft) / r_l;
            float ty = -(inTop + inBottom) / t_b;
            float tz = -(inFar + inNear) / f_n;

            return Mat44(
                Vec4(2.0f / r_l,  0.0f,         0.0f,       0.0f),
                Vec4(0.0f,        2.0f / t_b,   0.0f,       0.0f),
                Vec4(0.0f,        0.0f,        -2.0f / f_n, 0.0f),
                Vec4(tx,          ty,           tz,         1.0f)
            );
        }, "left"_a, "right"_a, "bottom"_a, "top"_a, "near"_a, "far"_a, "Opengl: depth (-1, 1) orthographic matrix")

        .def_static("perspective_infinite_reverse_z",[](float inFovY, float inAspect, float inNear){
            float height = 1.0f / Tan(0.5f * inFovY);
            float width = height / inAspect;

            return Mat44(Vec4(width, 0.0f, 0.0f, 0.0f),Vec4(0.0f, height, 0.0f, 0.0f), Vec4(0.0f, 0.0f, 0, -1.0f), Vec4(0.0f, 0.0f, inNear, 0.0f));
        }, "fov_y"_a, "aspect"_a, "near"_a)
        .def_static("from_numpy", &numpy_to_mat44, "numpy_array"_a, "Convert a 4x4 NumPy array to Mat44,data is cloned")
        .def("to_numpy", [](const Mat44 &self) {
            return mat44_to_numpy(self);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"), "Convert Mat44 to a 4x4 NumPy array, data is cloned")

        .def("to_list", [](const Mat44 &self) {
            nb::list outV;
            for (size_t c = 0; c < 4; ++c)
                for (size_t r = 0; r < 4; ++r)
                    outV.append(self(r, c));

            return outV;
        }, "Store matrix to memory")

        .def("to_bytes", [](Mat44 &self){
            nb::bytes outV(&self(0, 0), sizeof(Mat44));
            return outV;
        })

        .def("__repr__", [](const Mat44 &self) {
            std::ostringstream ss;
            ss << "Mat44([";
            for (int row = 0; row < 4; row++) {
                ss << (row == 0 ? "[" : "       [");
                for (int col = 0; col < 4; col++) {
                    ss << self(row, col);
                    if (col < 3) ss << ", ";
                }
                ss << "]" << (row < 3 ? ",\n" : "])");
            }
            return nb::str(ss.str().c_str());
        });
}
