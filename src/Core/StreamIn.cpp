#include "Common.h"
#include <Jolt/Core/StreamIn.h>
#include <BindingUtility/Stream.h>

PythonStreamIn::PythonStreamIn(nb::object py_io_object) : m_py_io(py_io_object) {
    if (!nb::hasattr(m_py_io, "read")) {
        throw nb::type_error("Object provided to StreamIn must have a 'read' method that returns bytes.");
    }
}

void PythonStreamIn::ReadBytes(void *outData, size_t inNumBytes) {

    if (m_is_failed)
        return;

    try {
        nb::bytes read_data = nb::cast<nb::bytes>(m_py_io.attr("read")(inNumBytes));

        if (read_data.size() < inNumBytes) {
            m_is_eof = true;
            m_is_failed = true;
            return;
        }

        memcpy(outData, read_data.c_str(), inNumBytes);
    } catch (const nb::python_error &e) {
        m_is_failed = true;
    }
}

bool PythonStreamIn::IsEOF() const {
    return m_is_eof;
}

bool PythonStreamIn::IsFailed() const {
    return m_is_failed;
}

void BindStreamIn(nb::module_ &m) {
    nb::class_<StreamIn, NonCopyable>(m, "StreamIn", "Simple binary input stream");
}