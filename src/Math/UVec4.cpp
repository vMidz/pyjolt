#include "Common.h"
#include <Jolt/Math/UVec4.h>
#include <nanobind/stl/string.h>
#include <nanobind/ndarray.h>
#include <nanobind/operators.h>

using NumpyUVec4 = nb::ndarray<nb::numpy, uint32, nb::shape<4>, nb::device::cpu, nb::c_contig>;

UVec4 numpy_to_uvec4(const NumpyUVec4 &numpy_arr) {
    // Extract data from NumPy array
    const uint32 *data = numpy_arr.data();

    return UVec4(data[0], data[1], data[2], data[3]);
}

NumpyUVec4 uvec4_to_numpy(const UVec4 &self) {
    // Allocate memory
    float *data = new float[4];
    nb::capsule owner(data, [](void *p) noexcept {
        delete[] (float *)p;
    });

    data[0] = self.GetX();
    data[1] = self.GetY();
    data[2] = self.GetZ();
    data[3] = self.GetW();

    return NumpyUVec4(data, {4}, owner);
}

UVec4 sGatherInt4(const uint32 *inBase, UVec4Arg inOffsets, int Scale) {
    const uint8 *base = reinterpret_cast<const uint8 *>(inBase);
    uint32 x = *reinterpret_cast<const uint32 *>(base + inOffsets.GetX() * Scale);
    uint32 y = *reinterpret_cast<const uint32 *>(base + inOffsets.GetY() * Scale);
    uint32 z = *reinterpret_cast<const uint32 *>(base + inOffsets.GetZ() * Scale);
    uint32 w = *reinterpret_cast<const uint32 *>(base + inOffsets.GetW() * Scale);
    return UVec4(x, y, z, w);
}

UVec4 ArithmeticShiftRight(UVec4 &self, uint8_t Count)
{
	assert(Count <= 31);
	return UVec4(uint32(int32_t(self.mU32[0]) >> Count),
				 uint32(int32_t(self.mU32[1]) >> Count),
				 uint32(int32_t(self.mU32[2]) >> Count),
				 uint32(int32_t(self.mU32[3]) >> Count));
}

UVec4 LogicalShiftRight(UVec4 &self, uint8_t Count)
{
	assert(Count <= 31);
	return UVec4(self.mU32[0] >> Count, self.mU32[1] >> Count, self.mU32[2] >> Count, self.mU32[3] >> Count);
}

UVec4 LogicalShiftLeft(UVec4 &self, uint8_t Count)
{
	assert(Count <= 31);
	return UVec4(self.mU32[0] << Count, self.mU32[1] << Count, self.mU32[2] << Count, self.mU32[3] << Count);
}

void BindUVec4(nb::module_ &m) {
    nb::class_<UVec4>(m, "UVec4", "Four components of unsigned integer")
        // Constructors
        .def(nb::init<>())
        .def(nb::init<const UVec4 &>(), "vector"_a)
        .def(nb::init<uint32_t, uint32_t, uint32_t, uint32_t>(), "x"_a, "y"_a, "z"_a, "w"_a,
            "Create a vector from 4 integer components")

        // <0, 1, 2, 3>, "Swizzle: xyzw (identity)"
        .def("swizzle_xxxx", &UVec4::Swizzle<0, 0, 0, 0>, "Swizzle: xxxx")
        .def("swizzle_yyyy", &UVec4::Swizzle<1, 1, 1, 1>, "Swizzle: yyyy")
        .def("swizzle_zzzz", &UVec4::Swizzle<2, 2, 2, 2>, "Swizzle: zzzz")
        .def("swizzle_wwww", &UVec4::Swizzle<3, 3, 3, 3>, "Swizzle: wwww")
        .def("swizzle_wzyx", &UVec4::Swizzle<3, 2, 1, 0>, "Swizzle: wzyx")

        // Static methods
        .def_static("zero", &UVec4::sZero, "Vector with all zeros")
        .def_static("replicate", &UVec4::sReplicate, "value"_a, "Replicate int inV across all components")
        .def_static("load_int", &UVec4::sLoadInt, "value"_a,
            "Load 1 int from memory and place it in the X component, zeros Y, Z and W")
        .def_static("load_int4", &UVec4::sLoadInt4, "vector"_a, "Load 4 ints from memory")
        .def_static("load_int4_aligned", &UVec4::sLoadInt4Aligned, "vecotr"_a, "Load 4 ints from memory, aligned to 16 bytes")
        .def_static("gather_int4", &sGatherInt4, "base"_a, "offsets"_a, "scale"_a,
            "Gather 4 ints from memory at inBase + inOffsets[i] * Scale")
        .def_static("min", &UVec4::sMin, "vector_1"_a, "vector_2"_a, "Return the minimum value of each of the components")
        .def_static("max", &UVec4::sMax, "vector_1"_a, "vector_2"_a, "Return the maximum of each of the components")
        .def_static("equals", &UVec4::sEquals, "vector_1"_a, "vector_2"_a, "Equals (component wise)")
        .def_static("select", &UVec4::sSelect,
            "not_set"_a, "set"_a, "control"_a,
            "Component wise select, returns inNotSet when highest bit of inControl = 0 and inSet when highest bit of inControl = 1")
        .def_static("or_", &UVec4::sOr, "vector_1"_a, "vector_2"_a, "Logical or (component wise)") // Underscore to avoid Python keyword conflict
        .def_static("xor", &UVec4::sXor, "vector_1"_a, "vector_2"_a, "Logical xor (component wise)")
        .def_static("and_", &UVec4::sAnd, "vector_1"_a, "vector_2"_a, "Logical and (component wise)")
        .def_static("not_", &UVec4::sNot, "vector_1"_a, "Logical not (componen t wise)") // Underscore to avoid Python keyword conflict
        .def_static("sort4_true", &UVec4::sSort4True, "value"_a, "index"_a,
            "Sorts the elements in inIndex so that the values that correspond to trues in \n"
            "inValue are the first elements. The remaining elements will be set to inValue.w.\n"
            "I.e. if inValue = (true, false, true, false) and inIndex = (1, 2, 3, 4) the function returns (1, 3, 4, 4).")

        .def_prop_rw("x", &UVec4::GetX, &UVec4::SetX, "x"_a, "Set/get component x")
        .def_prop_rw("y", &UVec4::GetY, &UVec4::SetY, "y"_a, "Set/get component y")
        .def_prop_rw("z", &UVec4::GetZ, &UVec4::SetZ, "z"_a, "Set/get component z")
        .def_prop_rw("w", &UVec4::GetW, &UVec4::SetW, "w"_a, "Set/get component w")

        .def("splat_x", &UVec4::SplatX, "Replicate the X component to all components")
        .def("splat_y", &UVec4::SplatY, "Replicate the Y component to all components")
        .def("splat_z", &UVec4::SplatZ, "Replicate the Z component to all components")
        .def("splat_w", &UVec4::SplatW, "Replicate the W component to all components")

        .def("to_float", &UVec4::ToFloat, "Convert each component from an int to a float")
        .def("reinterpret_as_float", &UVec4::ReinterpretAsFloat, "Reinterpret UVec4 as a Vec4 (doesn't change the bits)")
        .def("store_int4", &UVec4::StoreInt4, "out"_a, "Store 4 ints to memory")
        .def("store_int4_aligned", &UVec4::StoreInt4Aligned, "out"_a, "Store 4 ints to memory, aligned to 16 bytes")
        .def("test_any_true", &UVec4::TestAnyTrue, "Test if any of the components are true (true is when highest bit of component is set)")
        .def("test_any_xyz_true", &UVec4::TestAnyXYZTrue, "Test if any of X, Y or Z components are true (true is when highest bit of component is set)")
        .def("test_all_true", &UVec4::TestAllTrue, "Test if all components are true (true is when highest bit of component is set)")
        .def("test_all_xyz_true", &UVec4::TestAllXYZTrue, "Test if X, Y and Z components are true (true is when highest bit of component is set)")
        .def("count_trues", &UVec4::CountTrues, "Count the number of components that are true (true is when highest bit of component is set)")
        .def("GetTrues", &UVec4::GetTrues, "Store if X is true in bit 0, Y in bit 1, Z in bit 2 and W in bit 3 (true is when highest bit of component is set)")
        .def("logical_shift_left", &LogicalShiftLeft, "count"_a, "Shift all components by Count bits to the left (filling with zeros from the left)")
        .def("logical_shift_right", &LogicalShiftRight, "count"_a, "Shift all components by Count bits to the right (filling with zeros from the right)")
        .def("arithmetic_shift_right", &ArithmeticShiftRight, "count"_a, "Shift all components by Count bits to the right (shifting in the value of the highest bit)")
        .def("expand4_uint16_lo", &UVec4::Expand4Uint16Lo, "Takes the lower 4 16 bits and expands them to X, Y, Z and W")
        .def("expand4_uint16_hi", &UVec4::Expand4Uint16Hi, "Takes the upper 4 16 bits and expands them to X, Y, Z and W")
        .def("expand4_byte0", &UVec4::Expand4Byte0, "Takes byte 0 .. 3 and expands them to X, Y, Z and W")
        .def("expand4_byte4", &UVec4::Expand4Byte4, "Expand4Byte4")
        .def("expand4_byte8", &UVec4::Expand4Byte8, "Takes byte 8 .. 11 and expands them to X, Y, Z and W")
        .def("expand4_byte12", &UVec4::Expand4Byte12, "Takes byte 12 .. 15 and expands them to X, Y, Z and W")
        .def("shift_components4_minus", &UVec4::ShiftComponents4Minus, "count"_a,
            "Shift vector components by 4 - Count floats to the left, so if Count = 1 the resulting vector is (W, 0, 0, 0), when Count = 3 the resulting vector is (Y, Z, W, 0)")

        // Operators
        .def("__getitem__", [](const UVec4 &self, int i) {
            if (i >= 4)
                throw nb::index_error();
            return self[i];
        }, "index"_a, "Get component by index")
        .def("__setitem__", [](UVec4 &self, int i, uint32_t v) {
            if (i >= 4)
                throw nb::index_error();
            self[i] = v;
        }, "index"_a, "value"_a, "Set component by index")

        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a, "Comparison")

        .def(nb::self * nb::self, "rhs"_a, "Multiplies each of the 4 integer components with an integer (discards any overflow)")
        .def("__add__", [](UVec4 &self, UVec4 &rhs) {
            return self + rhs;
        }, "rhs"_a, "Adds an integer value to all integer components (discards any overflow)")

        .def(nb::self += nb::self, "rhs"_a, "Adds an integer value to all integer components (discards any overflow)")

        // Python NumPy integration
        .def_static("from_numpy", &numpy_to_uvec4, "numpy_array"_a, "Convert NumPy array to UVec4, data is cloned")
        .def("to_numpy", [](const UVec4 &self) {
            return uvec4_to_numpy(self);
        }, nb::sig("def to_numpy(self) -> numpy.ndarray"),
             "Convert UVec4 to a NumPy array, data is cloned")

        .def("__repr__", [](const UVec4 &self) {
            return nb::str("UVec4({}, {}, {}, {})").format(self.GetX(), self.GetY(), self.GetZ(), self.GetW());
        });
}