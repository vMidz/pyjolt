#include "Common.h"
#include <Jolt/Core/StreamOut.h>
#include <BindingUtility/Stream.h>

PythonStreamOut::PythonStreamOut(nb::object py_io_object) : m_py_io(py_io_object) {

    if (!nb::hasattr(m_py_io, "write")) {
        throw nb::type_error("Object provided to StreamOut must have a 'write' method that accepts bytes.");
    }
}

void PythonStreamOut::WriteBytes(const void *inData, size_t inNumBytes) {

    if (m_failed)
        return;

    try {
        nb::bytes data_view(inData, inNumBytes);

        m_py_io.attr("write")(data_view);
    } catch (const nb::python_error &e) {
        m_failed = true;
    }
}

bool PythonStreamOut::IsFailed() const {
    return m_failed;
}

void BindStreamOut(nb::module_ &m) {
    nb::class_<StreamOut, NonCopyable>(m, "StreamOut", "Simple binary output stream");
}