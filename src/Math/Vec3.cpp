#include "Common.h"
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/UVec4.h>
#include "BindingUtility/Tuple.h"
#include <nanobind/stl/string.h>
#include <nanobind/operators.h>
#include <nanobind/ndarray.h>

using NumpyVec3 = nb::ndarray<nb::numpy, float, nb::shape<3>, nb::device::cpu, nb::c_contig>;

Vec3 numpy_to_vec3(const NumpyVec3 &numpy_arr) {
    // Extract data from NumPy array
    const auto *data = numpy_arr.data();

    return Vec3(data[0], data[1], data[2]);
}

NumpyVec3 vec3_to_numpy(const Vec3 &self) {
    // Allocate memory
    float *data = new float[3];
    nb::capsule owner(data, [](void *p) noexcept {
        delete[] (float *)p;
    });

    data[0] = self.GetX();
    data[1] = self.GetY();
    data[2] = self.GetZ();

    return NumpyVec3(data, {3}, owner);
}

Vec3 sRandom(int seed)
{
    std::mt19937 generator(seed);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);

    float theta = JPH_PI * zero_to_one(generator);
    float phi = 2.0f * JPH_PI * zero_to_one(generator);

    return Vec3::sUnitSpherical(theta, phi);
}

void BindVec3(nb::module_ &m) {
    nb::class_<Vec3>(m, "Vec3", "3 component vector (stored as 4 vectors)\n"
                                "Note that we keep the 4th component the same as the 3rd component to avoid divisions by zero "
                                "when JPH_FLOATING_POINT_EXCEPTIONS_ENABLED defined")
        // Constructors
        .def(nb::init<>())
        .def(nb::init<const Vec3 &>(), "rhs"_a)
        .def(nb::init<float, float, float>(), "x"_a, "y"_a, "z"_a, "Create a vector from 3 components")
        .def(nb::init<const Vec4 &>(), "vector"_a)
        .def(nb::init<const Float3&>(), "vector"_a, "Load 3 floats from memory")

        // Static methods
        .def_static("zero", &Vec3::sZero, "Vector with all zeros")
        .def_static("nan", &Vec3::sNaN, "Vector with all NaN's")
        .def_static("axis_x", &Vec3::sAxisX, "Vectors with the principal axis")
        .def_static("axis_y", &Vec3::sAxisY)
        .def_static("axis_z", &Vec3::sAxisZ)
        .def_static("replicate", &Vec3::sReplicate, "value"_a, "Replicate inV across all components")
        .def_static("load_float3_unsafe", &Vec3::sLoadFloat3Unsafe, "v"_a, "Load 3 floats from memory (reads 32 bits extra which it doesn't use)")
        .def_static("min", &Vec3::sMin, "vector_1"_a, "vector_2"_a, "Return the minimum value of each of the components")
        .def_static("max", &Vec3::sMax, "vector_1"_a, "vector_2"_a, "Return the maximum of each of the components")
        .def_static("clamp", &Vec3::sClamp, "v"_a, "min"_a, "max"_a, "Clamp a vector between min and max (component wise)")
        .def_static("equals", &Vec3::sEquals, "vector_1"_a, "vector_2"_a, "Equals (component wise)")
        .def_static("less", &Vec3::sLess, "vector_1"_a, "vector_2"_a, "Less than (component wise)")
        .def_static("less_or_equal", &Vec3::sLessOrEqual, "vector_1"_a, "vector_2"_a, "Less than or equal (component wise)")
        .def_static("greater", &Vec3::sGreater, "vector_1"_a, "vector_2"_a, "Greater than (component wise)")
        .def_static("greater_or_equal", &Vec3::sGreaterOrEqual,
            "vector_1"_a, "vector_2"_a, "Greater than or equal (component wise)")
        .def_static("fused_multiply_add", &Vec3::sFusedMultiplyAdd,
            "mul_1"_a, "mul_2"_a, "add"_a, "Calculates inMul1 * inMul2 + inAdd")
        .def_static("select", &Vec3::sSelect, "not_set"_a, "set"_a, "control"_a,
            "Component wise select, returns inNotSet when highest bit of inControl = 0 and inSet when highest bit of inControl = 1")
        .def_static("or_", &Vec3::sOr,
            "vector_1"_a, "vector_2"_a, "Logical or (component wise)") // Added underscore to avoid conflic with Python keyword "or"
        .def_static("xor", &Vec3::sXor, "vector_1"_a, "vector_2"_a, "Logical xor (component wise)")
        .def_static("and_", &Vec3::sAnd, "vector_1"_a, "vector_2"_a, "Logical and (component wise)")
        .def_static("unit_spherical", &Vec3::sUnitSpherical,
            "theta"_a, "phi"_a, "Get unit vector given spherical coordinates\n"
            "inTheta \\f$\\in [0, \\pi]\\f$ is angle between vector and z-axis\n"
            "inPhi \\f$\\in [0, 2 \\pi]\\f$ is the angle in the xy-plane starting from the x axis and rotating counter clockwise around the z-axis")
        .def_ro_static("unit_sphere", &Vec3::sUnitSphere, "A set of vectors uniformly spanning the surface of a unit sphere, usable for debug purposes")
        .def_static("random", []() {
            static std::default_random_engine rd;
            return Vec3::sRandom(rd);
        }, "Get random unit vector, uses std::default_random_engine")

        // Component getters/setters
        .def_prop_rw("x", &Vec3::GetX, &Vec3::SetX, "x"_a, "Set individual components")
        .def_prop_rw("y", &Vec3::GetY, &Vec3::SetY, "y"_a)
        .def_prop_rw("z", &Vec3::GetZ, &Vec3::SetZ, "z"_a)
        .def("set", &Vec3::Set, "x"_a, "y"_a, "z"_a, "Set all components")

        .def("is_close", &Vec3::IsClose, "vector"_a, "max_dist_sq"_a = 1.0e-12f, "Test if two vectors are close")
        .def("is_near_zero", &Vec3::IsNearZero, "max_dist_sq"_a = 1.0e-12f, "Test if vector is near zero")
        .def("is_normalized", &Vec3::IsNormalized, "tolerance"_a = 1.0e-6f, "Test if vector is normalized")
        .def("is_nan", &Vec3::IsNaN, "Test if vector contains NaN elements")

        //<0, 1, 2>, "Swizzle: xyz (identity)
        .def("swizzle_xxx", &Vec3::Swizzle<0, 0, 0>, "Swizzle: xxx")
        .def("swizzle_yyy", &Vec3::Swizzle<1, 1, 1>, "Swizzle: yyy")
        .def("swizzle_zzz", &Vec3::Swizzle<2, 2, 2>, "Swizzle: zzz")
        .def("swizzle_zyx", &Vec3::Swizzle<2, 1, 0>, "Swizzle: zyx")

        .def("splat_x", &Vec3::SplatX, "Replicate the X component to all components")
        .def("splat_y", &Vec3::SplatY, "Replicate the Y component to all components")
        .def("splat_z", &Vec3::SplatZ, "Replicate the Z component to all components")
        .def("get_lowest_component_index", &Vec3::GetLowestComponentIndex, "Get index of component with lowest value")
        .def("get_highest_component_index", &Vec3::GetHighestComponentIndex, "Get index of component with highest value")
        .def("abs", &Vec3::Abs, "Return the absolute value of each of the components")
        .def("reciprocal", &Vec3::Reciprocal, "Reciprocal vector (1 / value) for each of the components")
        .def("cross", &Vec3::Cross, "vector"_a, "Cross product")
        .def("dot_v", &Vec3::DotV, "vector"_a, "Dot product, returns the dot product in X, Y and Z components")
        .def("dot_v4", &Vec3::DotV4, "vector"_a, "Dot product, returns the dot product in X, Y, Z and W components")
        .def("dot", &Vec3::Dot, "vector"_a, "Dot product")
        .def("length_sq", &Vec3::LengthSq, "Squared length of vector")
        .def("length", &Vec3::Length, "Length of vector")
        .def("normalized", &Vec3::Normalized, "Normalize vector")
        .def("normalized_or", &Vec3::NormalizedOr, "zero_value"_a, "Normalize vector or return inZeroValue if the length of the vector is zero")
        .def("store_float3", &Vec3::StoreFloat3, "out"_a, "Store 3 floats to memory")
        .def("to_int", &Vec3::ToInt, "Convert each component from a float to an int")
        .def("reinterpret_as_int", &Vec3::ReinterpretAsInt, "Reinterpret Vec3 as a UVec4 (doesn't change the bits)")
        .def("reduce_min", &Vec3::ReduceMin, "Get the minimum of X, Y and Z")
        .def("reduce_max", &Vec3::ReduceMax, "Get the maximum of X, Y and Z")
        .def("sqrt", &Vec3::Sqrt, "Component wise square root")
        .def("get_normalized_perpendicular", &Vec3::GetNormalizedPerpendicular, "Get normalized vector that is perpendicular to this vector")
        .def("get_sign", &Vec3::GetSign, "Get vector that contains the sign of each element (returns 1.0f if positive, -1.0f if negative)")
        .def("check_w", &Vec3::CheckW, "Internal helper function that checks that W is equal to Z, so e.g. dividing by it should not generate div by 0")
        .def_static("fix_w", [](const Vec3 &value){
            return Vec3(Vec3::sFixW(value.mValue));
        }, "value"_a, "Internal helper function that ensures that the Z component is replicated to the W component to prevent divisions by zero") // Not used in Python

        // Operators
        .def("__getitem__", [](const Vec3 &self, int i) {
            if (i >= 3)
                throw nb::index_error();
            return self[i];
        }, "index"_a, "Get float component by index")
        .def("__setitem__", [](Vec3 &self, int i, float v) {
            if (i >= 3)
                throw nb::index_error();
            self.SetComponent(i, v);
        }, "index"_a, "value"_a, "Set float component by index")

        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a, "Comparison")

        .def(nb::self * nb::self, "rhs"_a, "Multiply two float vectors (component wise)")
        .def(nb::self * float(), "rhs"_a, "Multiply vector with float")
        .def(float() * nb::self, "rhs"_a, "Multiply vector with float")
        .def(nb::self *= nb::self, "rhs"_a, "Multiply two float vectors (component wise)")
        .def(nb::self *= float(), "rhs"_a, "Multiply vector with float")

        .def(nb::self / nb::self, "rhs"_a, "Divide (component wise)")
        .def(nb::self / float(), "rhs"_a, "Divide vector by float")
        .def(nb::self /= float(), "rhs"_a, "Divide vector by float")

        .def(nb::self + nb::self, "rhs"_a, "Add two float vectors (component wise)")
        .def(nb::self += nb::self, "rhs"_a, "Add two float vectors (component wise)")
        .def(nb::self - nb::self, "rhs"_a, "Subtract two float vectors (component wise)")
        .def(nb::self -= nb::self, "rhs"_a, "Subtract two float vectors (component wise)")

        .def(-nb::self, "Negate")

        // Python-specific
        .def_static("from_numpy", &numpy_to_vec3, "numpy_array"_a, "Convert NumPy array to Vec3, data is cloned")
        .def("to_numpy", [](const Vec3 &self) {
            return vec3_to_numpy(self);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"),
           "Convert Vec3 to a NumPy array, data is cloned")

        .def("to_tuple", [](const Vec3 &self) {
            return CreateTuple(self.GetX(), self.GetY(), self.GetZ());
        })
        .def_static("from_tuple", [](const nb::typed<nb::tuple, int, int, int> &tuple) {
            return Vec3(nb::cast<float>(tuple[0]), nb::cast<float>(tuple[1]), nb::cast<float>(tuple[2]));
        }, "tuple"_a)

        .def("to_bytes", [](Vec3 &self){
            nb::bytes out(&self.mValue, 3 * sizeof(float));
            return out;
        })

        .def("__repr__", [](const Vec3 &self) {
            return nb::str("Vec3({}, {}, {})").format(self.GetX(), self.GetY(), self.GetZ());
        });
}