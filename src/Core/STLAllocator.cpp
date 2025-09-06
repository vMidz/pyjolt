#include "Common.h"
#include <Jolt/Core/STLAllocator.h>
#include <nanobind/operators.h>

// Custom Allocators are disabled
static void BindInst(nb::module_ &m, const char *name) {
    // using CurrentInst = STLAllocator<T>;
    // nb::class_<CurrentInst> cls(m, name, "STL allocator that forwards to our allocation functions");
    // cls
    //     .def(nb::init<>(), "Constructor")
    //     .def(nb::init<const CurrentInst &>(), "allocator"_a, "Constructor from other allocator")
    //     .def("allocate", &CurrentInst::allocate, "Allocate memory")
    //     .def("reallocate", &CurrentInst::reallocate, "Reallocate memory")
    //     .def("deallocate", &CurrentInst::deallocate, "Free memory")
    //     .def(nb::self == nb::self, "rhs"_a, "Allocators are stateless so assumed to be equal")
    //     .def(nb::self != nb::self, "rhs"_a);
}

void BindSTLAllocator(nb::module_ &m) {
    // BindInst(m, "STLAllocator");
}