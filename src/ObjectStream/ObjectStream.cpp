#include "Common.h"
#include <Jolt/ObjectStream/ObjectStream.h>
#include <Jolt/Core/RTTI.h>
#include <Jolt/Core/StaticArray.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Core/RTTI.h>
#include <Jolt/Core/NonCopyable.h>
#include <Jolt/ObjectStream/SerializableAttribute.h>
#include <nanobind/stl/string.h>

void BindObjectStream(nb::module_ &m) {

    nb::class_<ObjectStream, NonCopyable> object_stream(m, "ObjectStream", "Base class for object stream input and output streams.");

    nb::enum_<ObjectStream::EStreamType>(object_stream, "EStreamType", "Base class for object stream input and output streams.")
        .value("BINARY", ObjectStream::EStreamType::Binary)
        .value("TEXT", ObjectStream::EStreamType::Text);

    nb::class_<IObjectStreamIn, ObjectStream>(m, "IObjectStreamIn")
        .def("read_data_type", &IObjectStreamIn::ReadDataType, "@name Input type specific operations")
        .def("read_name", &IObjectStreamIn::ReadName)
        .def("read_identifier", &IObjectStreamIn::ReadIdentifier)
        .def("read_count", &IObjectStreamIn::ReadCount)

        .def("read_primitive_data", nb::overload_cast<uint8 &>(&IObjectStreamIn::ReadPrimitiveData), "@name Read primitives")
        .def("read_primitive_data", nb::overload_cast<uint16 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<int &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<uint32 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<uint64 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<float &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<double &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<bool &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<String &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<Float3 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<Double3 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<Vec3 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<DVec3 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<Vec4 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<Quat &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<Mat44 &>(&IObjectStreamIn::ReadPrimitiveData))
        .def("read_primitive_data", nb::overload_cast<DMat44 &>(&IObjectStreamIn::ReadPrimitiveData))

        .def("read_class_data", &IObjectStreamIn::ReadClassData, "in_class_name"_a, "in_instance"_a, "@name Read compounds")
        // .def("read_pointer_data", &IObjectStreamIn::ReadPointerData, "in_rtti"_a, "in_pointer"_a, "in_ref_count_offset"_a = -1)
        ;

    nb::class_<IObjectStreamOut, ObjectStream>(m, "IObjectStreamIn", "Interface class for writing to an object stream")
        .def("write_data_type", &IObjectStreamOut::WriteDataType, "in_type"_a, "@name Output type specific operations")
        .def("write_name", &IObjectStreamOut::WriteName, "in_name"_a)
        .def("write_identifier", &IObjectStreamOut::WriteIdentifier, "in_identifier"_a)
        .def("write_count", &IObjectStreamOut::WriteCount, "in_count"_a)

        .def("write_primitive_data", nb::overload_cast<const uint8 &>(&IObjectStreamOut::WritePrimitiveData), "@name Write primitives")
        .def("write_primitive_data", nb::overload_cast<const uint16 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const int &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const uint32 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const uint64 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const float &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const double &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const bool &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const String &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const Float3 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const Double3 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const Vec3 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const DVec3 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const Vec4 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const Quat &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const Mat44 &>(&IObjectStreamOut::WritePrimitiveData))
        .def("write_primitive_data", nb::overload_cast<const DMat44 &>(&IObjectStreamOut::WritePrimitiveData))

        .def("write_pointer_data", &IObjectStreamOut::WritePointerData, "in_rtti"_a, "in_pointer"_a, "@name Write compounds")
        .def("write_class_data", &IObjectStreamOut::WriteClassData, "in_rtti"_a, "instance"_a)

        .def("hint_next_item", &IObjectStreamOut::HintNextItem, "@name Layout hints (for text output)")
        .def("hint_indent_up", &IObjectStreamOut::HintIndentUp)
        .def("hint_indent_down", &IObjectStreamOut::HintIndentDown);
}