#include "Common.h"
#include <Jolt/Core/FPFlushDenormals.h>

void BindFPFlushDenormals(nb::module_ &m) {
    nb::class_<FPFlushDenormals>(m, "FPFlushDenormals")
        .def(nb::init<>(), "Helper class that needs to be put on the stack to enable flushing denormals to zero\n"
                           "This can make floating point operations much faster when working with very small numbers")
        .def("__enter__", [](FPFlushDenormals &self) -> FPFlushDenormals & {
            return self;
        })
        .def("__exit__", [](FPFlushDenormals &self, nb::args args) -> bool {
            return false;
        });
}