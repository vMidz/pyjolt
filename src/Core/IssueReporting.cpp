#include "Common.h"
#include <Jolt/Core/IssueReporting.h>

nb::callable py_trace_func;
nb::callable py_assert_func;

// Wrapper that calls the Python function
static void TraceImpl(const char *inFMT, ...) {
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    py_trace_func(buffer);
}

// Wrapper that calls the Python function
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint inLine) {
    auto r = py_assert_func(inExpression, inMessage, inFile, inLine);
    if (r.is_valid())
        return nb::cast<bool>(r);

    return true;
};

void BindIssueReporting(nb::module_ &m) {
    nb::class_<AssertLastParam>(m, "AssertLastParam", "Helper functions to pass message on to failed function");
    m.def("trace",[](const char * str){
		Trace(str);
    });

    m.def("set_trace", [](nb::object pyFunc) {
        if (pyFunc.is_none()) {
            Trace = nullptr;
            py_trace_func.reset();
        } else {
            if (!nb::isinstance<nb::callable>(pyFunc))
                throw nb::type_error("set_trace: argument must be a callable or None");

            py_trace_func = nb::cast<nb::callable>(pyFunc);
            Trace = TraceImpl;
        }
    }, "trace_impl"_a.none(true), nb::sig("def set_trace(trace_impl: Callable[[str], None] | None) -> None"),
       "Trace function, needs to be overridden by application. This should output a line of text to the log / TTY.");

    m.def("set_assert", [](nb::object pyFunc) {
        if (pyFunc.is_none()) {
            AssertFailed = nullptr;
            py_assert_func.reset();
        } else {
            if (!nb::isinstance<nb::callable>(pyFunc))
                throw nb::type_error("set_assert: argument must be a callable or None");

            py_assert_func = nb::cast<nb::callable>(pyFunc);
            AssertFailed = AssertFailedImpl;
        }
    }, "assert_failed_impl"_a.none(true), nb::sig("def set_assert(assert_failed_impl: Callable[[str, str, str, int], bool] | None) -> bool"));

    m.def("assert_failed_param_helper", nb::overload_cast<const char *, const char *, uint, AssertLastParam>(&AssertFailedParamHelper),
        "expression"_a, "file"_a, "line"_a, "assert_last_param"_a);
    m.def("assert_failed_param_helper", nb::overload_cast<const char *, const char *, uint, const char *, AssertLastParam>(&AssertFailedParamHelper),
        "expression"_a, "file"_a, "line"_a, "message"_a, "assert_last_param"_a);
}