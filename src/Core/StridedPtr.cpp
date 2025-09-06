#include "Common.h"
#include <type_traits>

#include <Jolt/Core/StridedPtr.h>
#include <Jolt/Geometry/Plane.h>

#include <nanobind/operators.h>

template <typename T>
static void BindInst(nb::module_ &m, std::string name) {
    using Instance = StridedPtr<T>;
    nb::class_<Instance>cls(m, (name + "StridedPtr").c_str());
    cls
        .def(nb::init<>())
        .def(nb::init<T *, int>(), "ptr"_a, "stride"_a)

        .def("get_ptr", &Instance::GetPtr, nb::rv_policy::reference_internal, "Explicit conversion")
        .def("get_stride", &Instance::GetStride, "Get stride in bytes")

        .def("assign", [](Instance &self) {
        })
        .def("__getitem__", [](const Instance &arr, int i) {
            return arr[i];
        }, "index"_a);
        if constexpr (!std::is_const_v<T>){
            cls
            .def("__setitem__", [](Instance &arr, int i, const T &v) {
                arr[i] = v;
            }, "index"_a, "value"_a);
        }
        cls
        .def(nb::self + int(), "rhs"_a)
        .def(nb::self += int(), "rhs"_a)

        .def(nb::self - int(), "rhs"_a)
        .def(nb::self -= int(), "rhs"_a)
        .def(nb::self - nb::self, "rhs"_a, "Distance between two pointers in elements")

        .def(nb::self == nb::self, "rhs"_a, "Comparison operators")
        .def(nb::self != nb::self, "rhs"_a, "Comparison operators")
        .def(nb::self <= nb::self, "rhs"_a, "Comparison operators")
        .def(nb::self >= nb::self, "rhs"_a, "Comparison operators")
        .def(nb::self < nb::self, "rhs"_a, "Comparison operators")
        .def(nb::self > nb::self, "rhs"_a, "Comparison operators");
}

void BindStridedPtr(nb::module_ &m) {
    BindInst<int>(m, "int");
    BindInst<float>(m, "float");
    BindInst<Vec3>(m, "Vec3");
    BindInst<Plane>(m, "Plane");

    BindInst<const Plane>(m, "ConstPlane");
    BindInst<const Vec3>(m, "ConstVec3");
    BindInst<const float>(m, "Constfloat");
}