#include "Common.h"
#include <Jolt/Physics/StateRecorderImpl.h>

#include <nanobind/stl/string.h>

void BindStateRecorderImpl(nb::module_ &m) {
    nb::class_<StateRecorderImpl, StateRecorder> stateRecorderImplCls(m, "StateRecorderImpl",
        "Implementation of the StateRecorder class that uses a stringstream as underlying store and that implements checking if the state doesn't change upon reading",
        nb::is_final());
    stateRecorderImplCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<StateRecorderImpl &&>(), "rhs"_a)
        .def("write_bytes", &StateRecorderImpl::WriteBytes, "data"_a, "num_bytes"_a,
            "Write a string of bytes to the binary stream")
        .def("rewind", &StateRecorderImpl::Rewind,
            "Rewind the stream for reading")
        .def("clear", &StateRecorderImpl::Clear,
            "Clear the stream for reuse")
        .def("read_bytes", &StateRecorderImpl::ReadBytes, "data"_a, "num_bytes"_a,
            "Read a string of bytes from the binary stream")
        .def("is_eof", &StateRecorderImpl::IsEOF)
        .def("is_failed", &StateRecorderImpl::IsFailed)
        .def("is_equal", &StateRecorderImpl::IsEqual, "reference"_a,
            "Compare this state with a reference state and ensure they are the same")
        .def("get_data", &StateRecorderImpl::GetData,
            "Convert the binary data to a string")
        .def("get_data_size", &StateRecorderImpl::GetDataSize,
            "Get size of the binary data in bytes");
}