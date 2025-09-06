#include "Common.h"
#include <Jolt/ObjectStream/ObjectStreamBinaryOut.h>
#include <Jolt/ObjectStream/ObjectStreamBinaryIn.h>
#include <Jolt/ObjectStream/ObjectStreamIn.h>

#include <nanobind/stl/string.h>

void BindObjectStreamBinaryOut(nb::module_ &m) {
    nb::class_<ObjectStreamBinaryOut, ObjectStreamOut>(m, "ObjectStreamBinaryOut", "Implementation of ObjectStream binary output stream.")
        .def(nb::init<ostream &>(), "stream"_a, "Constructor")

        .def("write_data_type", &ObjectStreamBinaryOut::WriteDataType, "type"_a, "@name Output type specific operations")
        .def("write_name", &ObjectStreamBinaryOut::WriteName, "name"_a)
        .def("write_identifier", &ObjectStreamBinaryOut::WriteIdentifier, "identifier"_a)
        .def("write_identifier", &ObjectStreamBinaryOut::WriteCount, "count"_a)
        .def("write_identifier", nb::overload_cast<const uint8 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const uint16 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const int &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const uint32 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const uint64 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const float &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const double &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const bool &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const String &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const Float3 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const Double3 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const Vec3 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const DVec3 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const Vec4 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const Quat &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const Mat44 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a)
        .def("write_identifier", nb::overload_cast<const DMat44 &>(&ObjectStreamBinaryOut::WritePrimitiveData), "primitive"_a);
}