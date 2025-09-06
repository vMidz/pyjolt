#include "Common.h"
#include <Jolt/ObjectStream/ObjectStreamTextOut.h>

void BindObjectStreamTextOut(nb::module_ &m) {
    nb::class_<ObjectStreamTextOut, ObjectStreamOut>(m, "ObjectStreamTextIn", "Implementation of ObjectStream text output stream.")
        .def(nb::init<ostream &>(), "stream"_a, "Constructor")
        .def("WriteDataType", &ObjectStreamTextOut::WriteDataType, "type"_a, "@name Output type specific operations")
        .def("write_name", &ObjectStreamTextOut::WriteName, "name"_a)
        .def("write_identifier", &ObjectStreamTextOut::WriteIdentifier, "identifier"_a)
        .def("write_count", &ObjectStreamTextOut::WriteCount, "count"_a)

        .def("write_primitive_data", nb::overload_cast<const uint8 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const uint16 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const int &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const uint32 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const uint64 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const float &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const double &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const bool &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const String &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const Float3 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const Double3 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const Vec3 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const DVec3 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const Vec4 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const Quat &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const Mat44 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)
        .def("write_primitive_data", nb::overload_cast<const DMat44 &>(&ObjectStreamTextOut::WritePrimitiveData), "primitive"_a)

        .def("hint_next_item", &ObjectStreamTextOut::HintNextItem, "@name Layout hints (for text output)")
        .def("hint_indent_up", &ObjectStreamTextOut::HintIndentUp)
        .def("hint_indent_down", &ObjectStreamTextOut::HintIndentDown);
}