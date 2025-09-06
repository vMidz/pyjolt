#include "Common.h"
#include <Jolt/Core/STLAlignedAllocator.h>
#include <nanobind/operators.h>

template <typename T, int N>
static void BindInst(nb::module_ &m, const char *name) {
    using CurrentInst = STLAlignedAllocator<T, N>;
    nb::class_<CurrentInst> cls(m, name, "STL allocator that takes care that memory is aligned to N bytes");
    cls
        .def(nb::init<>(), "Constructor")
        .def(nb::init<const CurrentInst &>(), "allocator"_a, "Constructor from other allocator")
        .def("allocate", &CurrentInst::allocate, "Allocate memory")
        .def("deallocate", &CurrentInst::deallocate, "Free memory")
        .def(nb::self == nb::self, "rhs"_a, "Allocators are stateless so assumed to be equal")
        .def(nb::self != nb::self, "rhs"_a);
}

void BindSTLAlignedAllocator(nb::module_ &m) {
    BindInst<int, 1>(m, "STLAlignedAllocatorInt1");
}