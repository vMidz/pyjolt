#include "Common.h"
#include <Jolt/ObjectStream/SerializableAttribute.h>
#include <Jolt/ObjectStream/ObjectStream.h>
#include <Jolt/ObjectStream/TypeDeclarations.h>
#include <Jolt/Core/RTTI.h>

void BindSerializableAttribute(nb::module_ &m) {
    nb::enum_<EOSDataType>(m, "EStreamType", "Data type")
        .value("Declare", EOSDataType::Declare, "Used to declare the attributes of a new object type")
        .value("Object", EOSDataType::Object, "Start of a new object")
        .value("Instance", EOSDataType::Instance, "Used in attribute declaration, indicates that an object is an instanced attribute (no pointer)")
        .value("Pointer", EOSDataType::Pointer, "Used in attribute declaration, indicates that an object is a pointer attribute")
        .value("Array", EOSDataType::Array, "Used in attribute declaration, indicates that this is an array of objects")
        .value("T_uint8", (EOSDataType)5)
        .value("T_uint16", (EOSDataType)6)
        .value("T_int", (EOSDataType)7)
        .value("T_uint32", (EOSDataType)8)
        .value("T_uint64", (EOSDataType)9)
        .value("T_float", (EOSDataType)10)
        .value("T_bool", (EOSDataType)11)
        .value("T_String", (EOSDataType)12)
        .value("T_Float3", (EOSDataType)13)
        .value("T_Vec3", (EOSDataType)14)
        .value("T_Vec4", (EOSDataType)15)
        .value("T_Quat", (EOSDataType)16)
        .value("T_Mat44", (EOSDataType)17)
        .value("T_double", (EOSDataType)18)
        .value("T_DVec3", (EOSDataType)19)
        .value("T_DMat44", (EOSDataType)20)
        .value("T_Double3", (EOSDataType)21)
        .value("invalid", EOSDataType::Invalid, "Next token on the stream was not a valid data type");

    nb::class_<SerializableAttribute>(m, "SerializableAttribute", "Attributes are members of classes that need to be serialized.")
        .def("__init__", [](SerializableAttribute &self, const char *inName, uint inMemberOffset) {
            // TODO:
            SerializableAttribute::pGetMemberPrimitiveType inGetMemberPrimitiveType;
            SerializableAttribute::pIsType inIsType;
            SerializableAttribute::pReadData inReadData;
            SerializableAttribute::pWriteData inWriteData;
            SerializableAttribute::pWriteDataType inWriteDataType;

            new (&self) SerializableAttribute(inName, inMemberOffset, inGetMemberPrimitiveType, inIsType, inReadData, inWriteData, inWriteDataType);
        }, "in_name"_a, "in_member_offset"_a, "Attributes are members of classes that need to be serialized.")
        .def(nb::init<const SerializableAttribute &, int>(), "other"_a, "base_offset"_a, "Construct from other attribute with base class offset")

        .def_prop_rw("name", &SerializableAttribute::GetName, &SerializableAttribute::SetName, "name"_a, "Get / set name")

        .def("get_member_pointer", [](SerializableAttribute &self, Vec3 vector) {
            auto r = self.GetMemberPointer<Vec3>(&vector);
            return nb::make_tuple(*r, vector);
        }, "vector"_a, "Output [return_value, input]")

        .def("get_member_primitive_type", &SerializableAttribute::GetMemberPrimitiveType, "In case this attribute contains an RTTI type, return it (note that a Array<sometype> will return the rtti of sometype)")
        .def("is_type", &SerializableAttribute::IsType, "array_depth"_a, "data_type"_a, "class_name"_a, "Check if this attribute is of a specific type")
        .def("read_data", &SerializableAttribute::ReadData,"stream"_a, "object"_a, "Read the data for this attribute into attribute containing class inObject")
        .def("write_data", &SerializableAttribute::WriteData, "stream"_a, "object"_a, "Write the data for this attribute from attribute containing class inObject")
        .def("write_data_type", &SerializableAttribute::WriteDataType, "stream"_a, "Write the data type of this attribute to a stream");
}