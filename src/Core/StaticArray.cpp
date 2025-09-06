#include "Common.h"
#include <Jolt/Core/StaticArray.h>
#include <Jolt/Geometry/EPAConvexHullBuilder.h>
#include <Jolt/Core/JobSystem.h>
#include <Jolt/Physics/Collision/EstimateCollisionResponse.h>

#include <nanobind/stl/string.h>
#include <nanobind/operators.h>
#include <nanobind/make_iterator.h>

template <typename T, uint N, bool HasEqualOP = true>
void BindStaticArray(nb::module_ &m, const char *name) {
    using CurrentArray = StaticArray<T, N>;

    nb::class_<CurrentArray> cls(m, name, "Simple variable length array backed by a fixed size buffer");
    cls
        .def(nb::init<>(), "Default constructor")
        .def(nb::init<const CurrentArray &>(), "Copy constructor", "other"_a)

        .def("clear", &CurrentArray::clear, "Destruct all elements and set length to zero")
        .def("push_back", &CurrentArray::push_back, "element"_a, "Add element to the back of the array")
        .def("emplace_back", [](CurrentArray &self, const T &value) {
            self.emplace_back(value);
        }, "value"_a, "Construct element at the back of the array")
        .def("pop_back", &CurrentArray::pop_back, "Remove element from the back of the array")
        .def("empty", &CurrentArray::empty, "Returns true if there are no elements in the array")
        .def("size", &CurrentArray::size, "Returns amount of elements in the array")
        .def("capacity", &CurrentArray::capacity, "Returns maximum amount of elements the array can hold")
        .def("resize", &CurrentArray::resize, "new_size"_a, "Resize array to new length")
        .def("begin", nb::overload_cast<>(&CurrentArray::begin))
        .def("end", nb::overload_cast<>(&CurrentArray::end))
        .def("data", nb::overload_cast<>(&CurrentArray::data), nb::rv_policy::reference_internal, "Get pointer to underlying data (const)")
        .def("at", nb::overload_cast<uint>(&CurrentArray::at), "Access element")
        .def("front", nb::overload_cast<>(&CurrentArray::front), nb::rv_policy::reference_internal, "First element in the array")
        .def("back", nb::overload_cast<>(&CurrentArray::back), nb::rv_policy::reference_internal, "Last element in the array")
        .def("erase", [](CurrentArray &self, uint idx) {
            if (idx >= self.size())
                throw nb::index_error("Index out of range");
            auto it = self.begin() + idx;
            self.erase(it);
        }, "index"_a, "Remove one element from the array")

        // Operators
        .def("__getitem__", [](const CurrentArray &self, uint idx) {
            if (idx >= self.size())
                throw nb::index_error("Index out of range");
            return self[idx];
        }, "Access element")
        .def("__setitem__", [](CurrentArray &self, uint idx, const T &value) {
            if (idx >= self.size())
                throw nb::index_error("Index out of range");
            self[idx] = value;
        }, "Access element");
        if constexpr (HasEqualOP) {
        cls
        .def(nb::self == nb::self, "rhs"_a, "Comparing arrays")
        .def(nb::self != nb::self, "rhs"_a, "Comparing arrays");
        }

        cls
        // Python-specific
        .def("erase_range", [](CurrentArray &self, uint begin_idx, uint end_idx) {
            if (begin_idx > self.size() || end_idx > self.size() || begin_idx > end_idx)
                throw nb::index_error("Invalid range");
            auto begin_it = self.begin() + begin_idx;
            auto end_it = self.begin() + end_idx;
            self.erase(begin_it, end_it);
        }, "begin_index"_a, "end_index"_a, "Remove elements in the specified range [begin, end)")

        .def("__len__", &CurrentArray::size)

        .def("__iter__", [](const CurrentArray &self) {
            return nb::make_iterator(nb::type<CurrentArray>(), "iterator", self.begin(), self.end());
        })

        .def_prop_ro_static("Capacity", [](nb::handle) {
            return CurrentArray::Capacity;
        }, "Maximum capacity of the array");
}

void BindStaticArray(nb::module_ &m) {
    BindStaticArray<Vec3, 32>(m, "StaticArrayVec3_32");

    // using PointsBase = StaticArray<Vec3, EPAConvexHullBuilder::cMaxPoints>;
    BindStaticArray<Vec3, EPAConvexHullBuilder::cMaxPoints>(m, "PointsBaseStaticArray");

    // using Triangles = StaticArray<Triangle *, cMaxTriangles>;
    BindStaticArray<EPAConvexHullBuilder::Triangle *, EPAConvexHullBuilder::cMaxTriangles>(m, "TrianglesStaticArray");

    BindStaticArray<Vec3, 1026>(m, "StaticArrayVec31026");

    BindStaticArray<Vec3, 64>(m, "Vec364");

    BindStaticArray<JobSystem::JobHandle, 32, false>(m, "JobSystemJobHandle32");
    BindStaticArray<CollisionEstimationResult::Impulse, 64, false>(m, "CollisionEstimationResultImpulse64");
}