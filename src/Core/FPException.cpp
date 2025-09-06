#include "Common.h"
#include <Jolt/Core/FPException.h>

void BindFPException(nb::module_ &m) {
    // m.attr("FP_IOE") = FP_IOE;
    // m.attr("FP_DZE") = FP_DZE;

    // Only enabled for MSVC
    nb::class_<FPExceptionsEnable>(m, "FPExceptionsEnable")
        .def(nb::init<>(), "Enable floating point divide by zero exception and exceptions on invalid numbers")
        .def("__enter__", [](FPExceptionsEnable &self) -> FPExceptionsEnable & {
            return self;
        })
        .def("__exit__", [](FPExceptionsEnable &self, nb::args args) -> bool {
            return false;
        });

    nb::class_<FPExceptionDisableInvalid>(m, "FPExceptionDisableInvalid")
        .def(nb::init<>(), "Disable invalid floating point value exceptions")
        .def("__enter__", [](FPExceptionDisableInvalid &self) -> FPExceptionDisableInvalid & {
            return self;
        })
        .def("__exit__", [](FPExceptionDisableInvalid &self, nb::args args) -> bool {
            return false;
        });

    nb::class_<FPExceptionDisableDivByZero>(m, "FPExceptionDisableDivByZero")
        .def(nb::init<>(), "Disable division by zero floating point exceptions")
        .def("__enter__", [](FPExceptionDisableDivByZero &self) -> FPExceptionDisableDivByZero & {
            return self;
        })
        .def("__exit__", [](FPExceptionDisableDivByZero &self, nb::args args) -> bool {
            return false;
        });
}