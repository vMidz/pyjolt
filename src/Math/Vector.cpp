#include "Common.h"
#include <Jolt/Math/Vector.h>
#include <nanobind/operators.h>
#include <nanobind/ndarray.h>

template <typename CurrentVector, uint OtherRows>
void BindCopyPart(nb::class_<CurrentVector> &cls) {
    using OtherVector = Vector<OtherRows>;
    cls.def("copy_part", [](CurrentVector &self, OtherVector &inV, uint sourceRow, uint numRows, uint destRow) {
        self.template CopyPart<OtherVector>(inV, sourceRow, numRows, destRow);
    }, "source"_a, "source_row"_a, "num_rows"_a, "dest_row"_a,
       "Copy part of another vector into this vector");
}

template <uint Rows>
void BindInst(nb::module_ &m, const char *name) {
    using CurrentVector = Vector<Rows>;
    using NumpyVector = nb::ndarray<nb::numpy, float, nb::shape<Rows>, nb::device::cpu, nb::c_contig>;

    nb::class_<CurrentVector> cls(m, name);
    cls
        .def(nb::init<>())
        .def("get_rows", &CurrentVector::GetRows, "Dimensions")
        .def("set_zero", &CurrentVector::SetZero, "Vector with all zeros")

        .def_static("zero", &CurrentVector::sZero, "Vector with all zeros")

        .def("is_close", &CurrentVector::IsZero, "Test if vector consists of all zeros")
        .def("is_close", &CurrentVector::IsClose, "vector"_a, "max_dist_sq"_a = 1.0e-12f, "Test if two vectors are close to each other")
        .def("dot", &CurrentVector::Dot, "vector"_a, "Dot product")
        .def("length_sq", &CurrentVector::LengthSq, "Squared length of vector")
        .def("length", &CurrentVector::Length, "Length of vector")
        .def("is_normalized", &CurrentVector::IsNormalized, "tolerance_sq"_a = 1.0e-6f, "Check if vector is normalized")
        .def("normalized", &CurrentVector::Normalized, "Normalize vector")

        // Operators
        .def("__getitem__", [](const CurrentVector &self, int i) {
            if (i < 0 || i >= 4)
                throw nb::index_error("Index out of range");
            return self[i];
        }, "index"_a, "Get float component by index")

        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a)

        .def(nb::self * float(), "rhs"_a, "Multiply vector with float")
        .def(float() * nb::self, "rhs"_a)
        .def(nb::self *= float(), "rhs"_a)
        .def(nb::self / float(), "rhs"_a, "Divide vector by float")
        .def(nb::self /= float(), "rhs"_a)

        .def(nb::self + nb::self, "rhs"_a, "Add two float vectors (component wise)")
        .def(nb::self += nb::self, "rhs"_a)

        .def(nb::self - nb::self, "rhs"_a, "Subtract two float vectors (component wise)")
        .def(nb::self -= nb::self, "rhs"_a)

        .def(-nb::self, "Negate")

        // Python-specific
        .def_static("from_numpy", [](const NumpyVector &arr) -> CurrentVector {
            const auto *data = arr.data();
            CurrentVector r;
            for (size_t i = 0; i < Rows; ++i)
                r[i] = data[i];
            return r;
        }, "numpy_array"_a, "Create a Matrix from a NumPy array of shape (Rows, Cols).")
        .def("to_numpy", [](const CurrentVector &self) -> NumpyVector {
            float *data = new float[Rows];
            nb::capsule owner(data, [](void *p) noexcept {
                delete[] (float *)p;
            });

            for (size_t i = 0; i < Rows; ++i) {
                data[i] = self[i];
            }

            return NumpyVector(data, {Rows}, owner);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"),
             "Convert the Matrix to a NumPy array of shape (Rows, Cols).")

        .def("__repr__", [](const CurrentVector &self) {
            std::ostringstream ss;
            ss << "Vector" << Rows << "(";
            for (uint i = 0; i < Rows; ++i) {
                ss << self[i];
                if (i < Rows - 1)
                    ss << ", ";
            }
            ss << ")";
            return nb::str(ss.str().c_str());
        });

    BindCopyPart<CurrentVector, 1>(cls);
    BindCopyPart<CurrentVector, 2>(cls);
    BindCopyPart<CurrentVector, 3>(cls);
    BindCopyPart<CurrentVector, 4>(cls);
}

void BindVector(nb::module_ &m) {
    BindInst<1>(m, "Vector1");
    BindInst<2>(m, "Vector2");
    BindInst<3>(m, "Vector3");
    BindInst<4>(m, "Vector4");
}