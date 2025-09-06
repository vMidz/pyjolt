#include "Common.h"
#include "BindingUtility/Tuple.h"
#include <nanobind/stl/string.h>
#include <nanobind/operators.h>
#include <nanobind/ndarray.h>

using NumpyVec4 = nb::ndarray<nb::numpy, float, nb::shape<4>, nb::device::cpu, nb::c_contig>;

Vec4 numpy_to_vec4(const NumpyVec4 &numpy_arr) {
    // Extract data from NumPy array
    const auto *data = numpy_arr.data();

    return Vec4(data[0], data[1], data[2], data[3]);
}

NumpyVec4 vec4_to_numpy(const Vec4 &self) {
    // Allocate memory
    float *data = new float[4];
    nb::capsule owner(data, [](void *p) noexcept {
        delete[] (float *)p;
    });

    data[0] = self.GetX();
    data[1] = self.GetY();
    data[2] = self.GetZ();
    data[3] = self.GetW();

    return NumpyVec4(data, {4}, owner);
}

Vec4 sGatherFloat4(const float *inBase, UVec4Arg inOffsets, int Scale) {
    const uint8 *base = reinterpret_cast<const uint8 *>(inBase);
    float x = *reinterpret_cast<const float *>(base + inOffsets.GetX() * Scale);
    float y = *reinterpret_cast<const float *>(base + inOffsets.GetY() * Scale);
    float z = *reinterpret_cast<const float *>(base + inOffsets.GetZ() * Scale);
    float w = *reinterpret_cast<const float *>(base + inOffsets.GetW() * Scale);
    return Vec4(x, y, z, w);
}

void BindVec4(nb::module_ &m) {
    nb::class_<Vec4>(m, "Vec4")
        // Constructors
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<Vec4>(), "vector"_a)
        .def(nb::init<Vec3>(), "vector"_a, "WARNING: W component undefined!")
        .def(nb::init<Vec3, float>(), "vector"_a, "w"_a)
        .def(nb::init<float, float, float, float>(), "x"_a, "y"_a, "z"_a, "w"_a, "Create a vector from 4 components")

        // Static methods
        .def_static("zero", &Vec4::sZero, "Vector with all zeros")
        .def_static("nan", &Vec4::sNaN, "Vector with all NaN's")
        .def_static("replicate", &Vec4::sReplicate, "value"_a, "Replicate value across all components")
        .def_static("load_float4", &Vec4::sLoadFloat4, "vector"_a, "Load 4 floats from memory")
        .def_static("load_float4_aligned", &Vec4::sLoadFloat4Aligned,
            "vector"_a, "Load 4 floats from memory, 16 bytes aligned")
        .def_static("gather_float4", &sGatherFloat4,
            "base"_a, "offsets"_a, "scale"_a, "Gather 4 floats from memory at inBase + inOffsets[i] * Scale")
        .def_static("min", &Vec4::sMin, "vector_1"_a, "vector_2"_a, "Return the minimum value of each of the components")
        .def_static("max", &Vec4::sMax, "vector_1"_a, "vector_2"_a, "Return the maximum of each of the components")
        .def_static("equals", &Vec4::sEquals, "vector_1"_a, "vector_2"_a, "Equals (component wise)")
        .def_static("less", &Vec4::sLess, "vector_1"_a, "vector_2"_a, "Less than (component wise)")
        .def_static("less_or_equal", &Vec4::sLessOrEqual, "vector_1"_a, "vector_2"_a, "Less than or equal (component wise)")
        .def_static("greater", &Vec4::sGreater,
            "vector_1"_a, "vector_2"_a, "Greater than (component wise)")
        .def_static("greater_or_equal", &Vec4::sGreaterOrEqual,
            "vector_1"_a, "vector_2"_a, "Greater than or equal (component wise)")
        .def_static("fused_multiply_add", &Vec4::sFusedMultiplyAdd,
            "mul_1"_a, "mul_2"_a, "add"_a, "Calculates inMul1 * inMul2 + inAdd")
        .def_static("select", &Vec4::sSelect, "not_set"_a, "set"_a, "contorl"_a,
            "Component wise select, returns inNotSet when highest bit of inControl = 0 and inSet when highest bit of inControl = 1")
        .def_static("or_", &Vec4::sOr, "vector_1"_a, "vector_2"_a, "Logical or (component wise))")
        .def_static("xor", &Vec4::sXor, "vector_1"_a, "vector_2"_a, "Logical xor (component wise)")
        .def_static("and_", &Vec4::sAnd, "vector_1"_a, "vector_2"_a, "Logical and (component wise)")
        .def_static("sort4", &Vec4::sSort4, "value"_a, "index"_a,
            "Sort the four elements of ioValue and sort ioIndex at the same time.\n"
            "Based on a sorting network: http://en.wikipedia.org/wiki/Sorting_network")
        .def_static("sort4_reverse", &Vec4::sSort4Reverse, "value"_a, "index"_a,
            "Reverse sort the four elements of ioValue (highest first) and sort ioIndex at the same time\n"
            "Based on a sorting network: http://en.wikipedia.org/wiki/Sorting_network")

        // Component access (get)
        .def_prop_rw("x", &Vec4::GetX, &Vec4::SetX, "x"_a, "Get x component")
        .def_prop_rw("y", &Vec4::GetY, &Vec4::SetY, "y"_a, "Get y component")
        .def_prop_rw("z", &Vec4::GetZ, &Vec4::SetZ, "z"_a, "Get z component")
        .def_prop_rw("w", &Vec4::GetW, &Vec4::SetW, "w"_a, "Get w component")
        .def("set", &Vec4::Set, "x"_a, "y"_a, "z"_a, "w"_a, "Set all components")

        .def("is_close", &Vec4::IsClose, "other"_a, "max_dist_sq"_a = 1.0e-12f, "Test if two vectors are close")
        .def("is_normalized", &Vec4::IsNormalized, "tolerance"_a = 1.0e-6f, "Test if vector is normalized")
        .def("is_nan", &Vec4::IsNaN, " Test if vector contains NaN elements")
        // Swizzling operations
        // <0, 1, 2, 3>, "Swizzle: xyzw (identity)"
        .def("swizzle_xxxx", &Vec4::template Swizzle<0, 0, 0, 0>, "Swizzle: xxxx")
        .def("swizzle_yyyy", &Vec4::template Swizzle<1, 1, 1, 1>, "Swizzle: yyyy")
        .def("swizzle_zzzz", &Vec4::template Swizzle<2, 2, 2, 2>, "Swizzle: zzzz")
        .def("swizzle_wwww", &Vec4::template Swizzle<3, 3, 3, 3>, "Swizzle: wwww")
        .def("swizzle_wzyx", &Vec4::template Swizzle<3, 2, 1, 0>, "Swizzle: wzyx")

        .def("splat_x", &Vec4::SplatX, "Replicate X component to all components")
        .def("splat_y", &Vec4::SplatY, "Replicate Y component to all components")
        .def("splat_z", &Vec4::SplatZ, "Replicate Z component to all components")
        .def("splat_w", &Vec4::SplatW, "Replicate W component to all components")
        .def("abs", &Vec4::Abs, "Return the absolute value of each of the components")
        .def("reciprocal", &Vec4::Reciprocal, "Reciprocal vector (1 / value) for each of the components")
        .def("dot_V", &Vec4::DotV, "other"_a, "Dot product, returns the dot product in X, Y and Z components")
        .def("dot", &Vec4::Dot, "other"_a, "Dot product")
        .def("length_sq", &Vec4::LengthSq, "Squared length of vector")
        .def("length", &Vec4::Length, "Length of vector")
        .def("normalized", &Vec4::Normalized, "Normalize vector")
        .def("store_float4", &Vec4::StoreFloat4, "out"_a, "Store 4 floats to memory")
        .def("to_int", &Vec4::ToInt, "Convert each component from a float to an int")
        .def("reinterpret_as_int", &Vec4::ReinterpretAsInt, "Reinterpret Vec4 as a UVec4 (doesn't change the bits)")
        .def("get_sign_bits", &Vec4::GetSignBits, "Store if X is negative in bit 0, Y in bit 1, Z in bit 2 and W in bit 3")
        .def("reduce_min", &Vec4::ReduceMin, "Get the minimum of X, Y, Z and W")
        .def("reduce_max", &Vec4::ReduceMax, "Get the maximum of X, Y, Z and W")
        .def("sqrt", &Vec4::Sqrt, "Component wise square root")
        .def("get_sign", &Vec4::GetSign,
            "Get vector that contains the sign of each element (returns 1.0f if positive, -1.0f if negative)")
        .def("sin_cos", [](const Vec4 &v) {
            Vec4 sin_result, cos_result;
            v.SinCos(sin_result, cos_result);
            return CreateTuple(sin_result, cos_result);
        }, "Calculate the sine and cosine for each element of this vector (input in radians)\n"
           "Returns:\n"
           "    tuple\n"
           "    - Vec4: Sin\n"
           "    - Vec4: Cos")

        .def("tan", &Vec4::Tan, "Calculate the tangent for each element of this vector (input in radians)")
        .def("asin", &Vec4::ASin,
            "Calculate the arc sine for each element of this vector (returns value in the range [-PI / 2, PI / 2])\n"
            "Note that all input values will be clamped to the range [-1, 1] and this function will not return NaNs like std::asin")
        .def("acos", &Vec4::ACos,
            "Calculate the arc cosine for each element of this vector (returns value in the range [0, PI])"
            "Note that all input values will be clamped to the range [-1, 1] and this function will not return NaNs like std::acos")
        .def("atan", &Vec4::ATan, "Calculate the arc tangent for each element of this vector (returns value in the range [-PI / 2, PI / 2])")
        .def_static("atan2", &Vec4::sATan2,
            "y"_a, "x"_a,
            "Calculate the arc tangent of y / x using the signs of the arguments to determine the correct quadrant (returns value in the range [-PI, PI])")

        // Operators
        .def("__getitem__", [](const Vec4 &v, int i) {
            if (i >= 4)
                throw nb::index_error("Index out of range");
            return v[i];
        },
             "index"_a)
        .def("__setitem__", [](Vec4 &v, int i, float value) {
            if (i >= 4)
                throw nb::index_error("Index out of range");
            v[i] = value;
        }, "index"_a, "value"_a)

        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a, "Comparison")

        .def(nb::self + nb::self, "rhs"_a, "Add two float vectors (component wise)")
        .def(nb::self += nb::self, "rhs"_a, "Add two float vectors (component wise)")
        .def(nb::self - nb::self, "rhs"_a, "Subtract two float vectors (component wise)")
        .def(nb::self -= nb::self, "rhs"_a, "Subtract two float vectors (component wise)")

        .def(nb::self * nb::self, "rhs"_a, "Multiply two float vectors (component wise)")
        .def(nb::self *= nb::self, "rhs"_a, "Multiply two float vectors (component wise)")
        .def(nb::self * float(), "rhs"_a, "Multiply vector with float")
        .def(nb::self *= float(), "rhs"_a, "Multiply vector with float")
        .def(float() * nb::self, "rhs"_a, "Multiply vector with float")
        .def(nb::self / nb::self, "rhs"_a, "Divide (component wise)")
        .def(nb::self / float(), "rhs"_a, "Divide vector by float")
        .def(nb::self /= float(), "rhs"_a, "Divide vector by float")

        .def(-nb::self, "Negate")

        // NumPy integration
        .def_static("from_numpy", &numpy_to_vec4, "Convert NumPy array to Vec4, data is cloned", "arr"_a)
        .def("to_numpy", [](const Vec4 &self) {
            return vec4_to_numpy(self);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"), "Convert Vec4 to a NumPy array, data is cloned")

        .def("to_bytes", [](Vec4 &self){
            nb::bytes out(&self.mValue, sizeof(Vec4));
            return out;
        })

        // String representation
        .def("__repr__", [](const Vec4 &self) {
            return nb::str("Vec4({}, {}, {}, {})").format(self.GetX(), self.GetY(), self.GetZ(), self.GetW());
        });
}