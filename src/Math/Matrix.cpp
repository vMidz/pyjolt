#include "Common.h"
#include <Jolt/Math/Matrix.h>
#include <nanobind/stl/string.h>
#include <nanobind/ndarray.h>
#include <nanobind/operators.h>
#include <sstream>
#include <Jolt/Math/Vector.h>

template <int Other, int Rows, int Cols>
static void BindMul(nb::class_<Matrix<Rows, Cols>> &m) {
    using CurrentMatrix = Matrix<Rows, Cols>;
    m.def("__mul__", [](CurrentMatrix &self, Matrix<Cols, Other> &rhs) -> Matrix<Rows, Other> {
        return self * rhs;
    }, "rhs"_a, "Multiply matrix by matrix");
}

template <int Rows, int Cols>
static void BindInst(nb::module_ &m, const char *name) {
    using CurrentMatrix = Matrix<Rows, Cols>;
    using NumpyMatrix = nb::ndarray<nb::numpy, float, nb::shape<4, 4>, nb::device::cpu, nb::c_contig>;

    nb::class_<CurrentMatrix> cls(m, name, "Matrix class");
    cls
        .def(nb::init<>(), "Default constructor")
        .def(nb::init<const CurrentMatrix &>(), "matrix"_a, "Copy constructor")
        .def("get_rows", &CurrentMatrix::GetRows, "Get number of rows")
        .def("get_cols", &CurrentMatrix::GetCols, "Get number of columns")
        .def("set_zero", &CurrentMatrix::SetZero, "Zero matrix")
        .def("is_zero", &CurrentMatrix::IsZero, "Check if this matrix consists of all zeros")
        .def("set_identity", &CurrentMatrix::SetIdentity, "Identity matrix")
        .def("is_identity", &CurrentMatrix::IsIdentity, "Check if this matrix is identity")
        .def("set_diagonal", &CurrentMatrix::SetDiagonal, "vector"_a, "Diagonal matrix")
        .def("transposed", &CurrentMatrix::Transposed, "Transpose the matrix")
        .def("inversed", &CurrentMatrix::Inversed, "Inverse matrix")
        .def("set_inversed", &CurrentMatrix::SetInversed, "matrix"_a, "Inverse matrix")

        .def("copy_part", &CurrentMatrix::template CopyPart<Matrix<2, 2>>,
            "matrix"_a, "source_row"_a, "source_col"_a, "num_row"_a, "num_col"_a, "dest_row"_a, "dest_col"_a,
            "Copy a (part) of another matrix into this matrix")
        .def("copy_part", &CurrentMatrix::template CopyPart<Matrix<3, 3>>,
            "matrix"_a, "source_row"_a, "source_col"_a, "num_row"_a, "num_col"_a, "dest_row"_a, "dest_col"_a,
            "Copy a (part) of another matrix into this matrix")
        .def("copy_part", &CurrentMatrix::template CopyPart<Matrix<4, 4>>,
            "matrix"_a, "source_row"_a, "source_col"_a, "num_row"_a, "num_col"_a, "dest_row"_a, "dest_col"_a,
            "Copy a (part) of another matrix into this matrix")
        .def("copy_part", &CurrentMatrix::template CopyPart<Matrix<3, 4>>,
            "matrix"_a, "source_row"_a, "source_col"_a, "num_row"_a, "num_col"_a, "dest_row"_a, "dest_col"_a,
            "Copy a (part) of another matrix into this matrix")
        .def("copy_part", &CurrentMatrix::template CopyPart<Matrix<4, 3>>,
            "matrix"_a, "source_row"_a, "source_col"_a, "num_row"_a, "num_col"_a, "dest_row"_a, "dest_col"_a,
            "Copy a (part) of another matrix into this matrix")

        .def("get_column", nb::overload_cast<int>(&CurrentMatrix::GetColumn), "index"_a, "Column access")

        .def_static("identity", &CurrentMatrix::sIdentity, "Identity matrix")
        .def_static("zero", &CurrentMatrix::sZero, "Zero matrix")
        .def_static("diagonal", &CurrentMatrix::sDiagonal, "vector"_a, "Diagonal matrix")

        // Operator overloads
        .def("__getitem__", [](const CurrentMatrix &m, nb::typed<nb::tuple, int, int> &index) {
            auto first = nb::cast<int>(index[0]);
            auto second = nb::cast<int>(index[1]);
            if (first >= Rows || second >= Cols)
                throw nb::index_error("Matrix index out of range");
            return m(first, second);
        }, "index"_a)
        .def("__setitem__", [](CurrentMatrix &m, nb::typed<nb::tuple, int, int> &index, float value) {
            auto first = nb::cast<int>(index[0]);
            auto second = nb::cast<int>(index[1]);
            if (first >= Rows || second >= Cols)
                throw nb::index_error("Matrix index out of range");
            m(first, second) = value;
        }, "index"_a, "value"_a)

        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a, "Comparison")

        .def(float() * nb::self, "rhs"_a, "Multiply matrix with float")
        .def(nb::self * float(), "rhs"_a, "Multiply matrix with float")
        .def(nb::self * Vector<Cols>(), "rhs"_a, "Multiply vector by matrix")

        .def(nb::self + nb::self, "rhs"_a, "Per element addition of matrix")
        .def(nb::self - nb::self, "rhs"_a, "Per element subtraction of matrix")

        // Python-specific
        .def_static("from_numpy", [](const NumpyMatrix &numpy_arr) -> CurrentMatrix {
            CurrentMatrix mat;
            const auto *data = numpy_arr.data();
            for (size_t c = 0; c < Cols; ++c)
                for (size_t r = 0; r < Rows; ++r)
                    mat(r, c) = data[r + (Cols * c)];

            return mat;
        }, "numpy_array"_a, "Create a Matrix from a NumPy array of shape (Rows, Cols).")
        .def("to_numpy", [](const CurrentMatrix &self) -> NumpyMatrix {
            float *data = new float[4 * 4];
            nb::capsule owner(data, [](void *p) noexcept {
                delete[] (float *)p;
            });

            for (size_t c = 0; c < 4; ++c)
                for (size_t r = 0; r < 4; ++r)
                    data[r + (4 * c)] = self(r, c);

            return NumpyMatrix(data, {4, 4}, owner);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"),
             "Convert the Matrix to a NumPy array of shape (Rows, Cols).")

        // String representation
        .def("__repr__", [](const CurrentMatrix &self) {
            std::ostringstream ss;
            ss << "Matrix([";
            for (int row = 0; row < Rows; row++) {
                ss << (row == 0 ? "[" : "       [");  // align subsequent rows
                for (int col = 0; col < Cols; col++) {
                    ss << self(row, col);
                    if (col < 3) ss << ", ";
                }
                ss << "]" << (row < 3 ? ",\n" : "])");
            }
            return nb::str(ss.str().c_str());
        });

    BindMul<2, Rows, Cols>(cls);
    BindMul<3, Rows, Cols>(cls);
    BindMul<4, Rows, Cols>(cls);
}

void BindMatrix(nb::module_ &m) {
    m.def("set_inversed", &Matrix<2, 2>::SetInversed, "lhs"_a, "rhs"_a, "Specialization of SetInversed for 2x2 matrix");

    BindInst<2, 2>(m, "Matrix2x2");
    BindInst<2, 3>(m, "Matrix2x3");
    BindInst<2, 4>(m, "Matrix2x4");

    BindInst<3, 2>(m, "Matrix3x2");
    BindInst<3, 3>(m, "Matrix3x3");
    BindInst<3, 4>(m, "Matrix3x4");

    BindInst<4, 2>(m, "Matrix4x2");
    BindInst<4, 3>(m, "Matrix4x3");
    BindInst<4, 4>(m, "Matrix4x4");
}