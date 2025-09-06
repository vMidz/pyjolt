#include "Common.h"
#include <nanobind/operators.h>
#include <Jolt/Math/DVec3.h>
#include <nanobind/ndarray.h>

using NumpyDVec3 = nb::ndarray<nb::numpy, double, nb::shape<4>, nb::device::cpu, nb::c_contig>;

DVec3 numpy_to_DVec3(const NumpyDVec3 &numpy_arr) {
    const double *data = numpy_arr.data();
    return DVec3(data[0], data[1], data[2]);
}

NumpyDVec3 DVec3_to_numpy(const DVec3 &self) {
    double *data = new double[4];
    nb::capsule owner(data, [](void *p) noexcept {
        delete[] (double *)p;
    });

    data[0] = self.GetX();
    data[1] = self.GetY();
    data[2] = self.GetZ();
    data[3] = self.GetZ(); // 4th componenet is the same as the 3rd

    return NumpyDVec3(data, {4}, owner);
}

void BindDVec3(nb::module_ &m) {
    nb::class_<DVec3>(m, "DVec3",
        "3 component vector of doubles (stored as 4 vectors).\n"
        "Note that we keep the 4th component the same as the 3rd component to avoid divisions by zero when JPH_FLOATING_POINT_EXCEPTIONS_ENABLED defined")
        // Constructors
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<DVec3>(), "vector"_a)
        .def(nb::init<Vec3>(), "vector"_a)
        .def(nb::init<Vec4>(), "vector"_a)
        .def(nb::init<double, double, double>(), "x"_a, "y"_a, "z"_a, "Create a vector from 3 components")
        .def(nb::init<Double3>(), "vector"_a, "Load 3 doubles from memory")

        .def_static("zero", &DVec3::sZero, "Vector with all zeros")
        .def_static("axis_x", &DVec3::sAxisX, "Vectors with the principal axis")
        .def_static("axis_y", &DVec3::sAxisY, "Vectors with the principal axis")
        .def_static("axis_z", &DVec3::sAxisZ, "Vectors with the principal axis")
        .def_static("replicate", &DVec3::sReplicate, "vector"_a, "Replicate inV across all components")
        .def_static("nan", &DVec3::sNaN, "Vector with all NaN's")
        .def_static("load_double3_unsafe", &DVec3::sLoadDouble3Unsafe,
            "vector"_a, "Load 3 doubles from memory (reads 64 bits extra which it doesn't use)")
        .def("store_double3", &DVec3::StoreDouble3, "out"_a, "Store 3 doubles to memory")
        .def("prepare_round_to_zero", &DVec3::PrepareRoundToZero,
            "Prepare to convert to float vector 3 rounding towards zero (returns DVec3 that can be converted to a Vec3 to get the rounding)")
        .def("prepare_round_to_inf", &DVec3::PrepareRoundToInf,
            "Prepare to convert to float vector 3 rounding towards positive/negative inf (returns DVec3 that can be converted to a Vec3 to get the rounding)")
        .def("to_vec3_round_down", &DVec3::ToVec3RoundDown, "Convert to float vector 3 rounding down")
        .def("to_vec3_round_up", &DVec3::ToVec3RoundUp, "Convert to float vector 3 rounding up")
        .def_static("min", &DVec3::sMin, "vector_1"_a, "vector_2"_a, "Return the minimum value of each of the components")
        .def_static("max", &DVec3::sMax, "vector_1"_a, "vector_2"_a, "Return the maximum of each of the components")
        .def_static("clamp", &DVec3::sClamp, "vector"_a, "min"_a, "max"_a, "Clamp a vector between min and max (component wise)")
        .def_static("equals", &DVec3::sEquals, "vector_1"_a, "vector_2"_a, "Equals (component wise)")
        .def_static("less", &DVec3::sLess, "vector_1"_a, "vector_2"_a, "Less than (component wise)")
        .def_static("less_or_equal", &DVec3::sLessOrEqual, "vector_1"_a, "vector_2"_a, "Less than or equal (component wise)")
        .def_static("greater", &DVec3::sGreater, "vector_1"_a, "vector_2"_a, "Greater than (component wise)")
        .def_static("greater_or_equal", &DVec3::sGreaterOrEqual, "vector_1"_a, "vector_2"_a, "Greater than or equal (component wise)")
        .def_static("fused_multiply_add", &DVec3::sFusedMultiplyAdd, "mul_1"_a, "mul_2"_a, "add"_a, "Calculates mul_1 * mul_2 + add")
        .def_static("select", &DVec3::sSelect, "not_set"_a, "set"_a, "control"_a,
            "Component wise select, returns inNotSet when highest bit of inControl = 0 and inSet when highest bit of inControl = 1")
        .def_static("or_", &DVec3::sOr, "vector_1"_a, "vector_2"_a, "Logical or (component wise)")    // Underscore is added since "or" is Python keyword
        .def_static("xor", &DVec3::sXor, "vector_1"_a, "vector_2"_a, "Logical xor (component wise)")
        .def_static("and_", &DVec3::sAnd, "vector_1"_a, "vector_2"_a, "Logical and (component wise)") // Underscore is added since "and" is Python keyword
        .def("get_trues", &DVec3::GetTrues,
            "Store if X is true in bit 0, Y in bit 1, Z in bit 2 and W in bit 3 (true is when highest bit of component is set)")
        .def("test_any_true", &DVec3::TestAnyTrue, "Test if any of the components are true (true is when highest bit of component is set)")
        .def("test_all_true", &DVec3::TestAllTrue, "Test if all components are true (true is when highest bit of component is set)")
        .def_prop_rw("x_axis", &DVec3::GetX, &DVec3::SetX, "x"_a)
        .def_prop_rw("y_axis", &DVec3::GetY, &DVec3::SetY, "y"_a)
        .def_prop_rw("z_axis", &DVec3::GetZ, &DVec3::SetZ, "z"_a)
        .def("set", &DVec3::Set, "x"_a, "y"_a, "z"_a, "Set all components")
        .def("set_component", &DVec3::SetComponent, "coordinate"_a, "value"_a, "Set double component by index")
        .def("is_close", &DVec3::IsClose, "other"_a, "max_dist_sq"_a = 1.0e-24, " Test if two vectors are close")
        .def("is_near_zero", &DVec3::IsNearZero, "max_dist_sq"_a = 1.0e-24, "Test if vector is near zero")
        .def("is_normalized", &DVec3::IsNormalized, "tolerance"_a = 1.0e-12, "Test if vector is normalized")
        .def("is_nan", &DVec3::IsNaN, "Test if vector contains NaN elements")
        .def("abs", &DVec3::Abs, "Return the absolute value of each of the components")
        .def("reciprocal", &DVec3::Reciprocal, "Reciprocal vector (1 / value) for each of the components")
        .def("cross", &DVec3::Cross, "vector_2"_a, "Cross product")
        .def("dot", &DVec3::Dot, "Dot product", "vector"_a)
        .def("length_sq", &DVec3::LengthSq, "Squared length of vector")
        .def("length", &DVec3::Length, "Length of vector")
        .def("normalized", &DVec3::Normalized, "Normalize vector")
        .def("sqrt", &DVec3::Sqrt, "Component wise square root")
        .def("get_sign", &DVec3::GetSign, "Get vector that contains the sign of each element (returns 1 if positive, -1 if negative)")
        .def("check_w", &DVec3::CheckW,
            "Internal helper function that checks that W is equal to Z, so e.g. dividing by it should not generate div by 0")
        .def_static("fix_w", [](const DVec3 &value){
            return DVec3(DVec3::sFixW(value.mValue));
        }, "value"_a,
           "Internal helper function that ensures that the Z component is replicated to the W component to prevent divisions by zero")
        .def_ro_static("c_true", &DVec3::cTrue, "Representations of true and false for boolean operations")
        .def_ro_static("c_false", &DVec3::cFalse)

        // Operators
        .def("__getitem__", [](const DVec3 &self, unsigned int i) {
            if (i >= 3)
                throw nb::index_error("Index out of range");
            return self[i];
        }, "Get double component by index", "index"_a)
        .def("to_vec3",  [](DVec3 &self){
           return Vec3(self);
        })

        .def(nb::self == nb::self, "rhs"_a)
        .def(nb::self != nb::self, "rhs"_a)

        .def(nb::self / double(), "rhs"_a)
        .def(nb::self /= double(), "rhs"_a)
        .def(nb::self / nb::self, "rhs"_a)

        .def(nb::self * double(), "rhs"_a)
        .def(double() * nb::self, "rhs"_a)
        .def(nb::self * nb::self, "rhs"_a)
        .def(nb::self *= double(), "rhs"_a)
        .def(nb::self *= nb::self, "rhs"_a)

        .def(nb::self + Vec3(), "rhs"_a)
        .def(nb::self + nb::self, "rhs"_a)
        .def(nb::self += Vec3(), "rhs"_a)
        .def(nb::self += nb::self, "rhs"_a)

        .def(nb::self - Vec3(), "rhs"_a)
        .def(nb::self - nb::self, "rhs"_a)
        .def(nb::self -= Vec3(), "rhs"_a)
        .def(nb::self -= nb::self, "rhs"_a)

        .def(-nb::self)

        // Python-specific
        .def_static("from_numpy", &numpy_to_DVec3, "numpy_array"_a, "Convert Numpy to DVec3, data is cloned")
        .def("to_numpy", [](const DVec3 &self) {
            return DVec3_to_numpy(self);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"), "Convert DVec3 to Numpy, data is cloned")

        .def("__repr__", [](const DVec3 &self) {
            return nb::str("DVec3({0}, {1}, {2})").format(self.GetX(), self.GetY(), self.GetZ());
        });
}