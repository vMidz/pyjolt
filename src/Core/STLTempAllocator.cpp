#include "Common.h"
#include <Jolt/Core/STLTempAllocator.h>
#include <nanobind/operators.h>

template <typename T>
static void BindInst(nb::module_ &m, const char *name) {
    using CurrentInst = STLTempAllocator<T>;
    nb::class_<CurrentInst> cls(m, name, "STL allocator that takes care that memory is aligned to N bytes");
    cls
        .def(nb::init<TempAllocator &>(), "allocator"_a, "Constructor")
        .def(nb::init<const CurrentInst &>(), "rhs"_a, "Constructor from other allocator")
        .def("allocate", &CurrentInst::allocate, "Allocate memory")
        .def("deallocate", &CurrentInst::deallocate, "Free memory")
        .def(nb::self == nb::self, "rhs"_a, "Allocators are not-stateless, assume if allocator address matches that the allocators are the same")
        .def(nb::self != nb::self, "rhs"_a)
        .def("get_allocator", &CurrentInst::GetAllocator, nb::rv_policy::reference_internal, "Get our temp allocator");
}

void BindSTLTempAllocator(nb::module_ &m) {
    BindInst<int>(m, "STLTempAllocatorInt");
}