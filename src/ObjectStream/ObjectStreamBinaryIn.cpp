#include "Common.h"
#include <Jolt/ObjectStream/ObjectStreamBinaryIn.h>
#include <nanobind/stl/string.h>

template <typename T>
auto ReadPrimitiveDataImpl() {
    return [](ObjectStreamBinaryIn &self) {
        T data;
        auto r = self.ReadPrimitiveData(data);
        return nb::make_tuple(r, data);
    };
}

void BindObjectStreamBinaryIn(nb::module_ &m) {
    nb::class_<ObjectStreamBinaryIn, ObjectStreamIn>(m, "ObjectStreamBinaryIn",
        "Implementation of ObjectStream binary input stream.")
        .def(nb::init<istream &>(), "in_stream"_a, "Constructor")

        .def("read_data_type", [](ObjectStreamBinaryIn &self) {
            EOSDataType out;
            self.ReadDataType(out);
            return out;
        }, "@name Input type specific operations")

        .def("read_name", [](ObjectStreamBinaryIn &self) {
            string out;
            self.ReadName(out);
            return out;
        })

        .def("read_identifier", [](ObjectStreamBinaryIn &self) {
            uint32 id;
            self.ReadIdentifier(id);
            return id;
        })

        .def("read_count", [](ObjectStreamBinaryIn &self) {
            uint32 count;
            self.ReadCount(count);
            return count;
        })

        .def("read_primitive_data_uint8", ReadPrimitiveDataImpl<uint8>())
        .def("read_primitive_data_uint16", ReadPrimitiveDataImpl<uint16>())
        .def("read_primitive_data_int", ReadPrimitiveDataImpl<int>())
        .def("read_primitive_data_uint32", ReadPrimitiveDataImpl<uint32>())
        .def("read_primitive_data_uint64", ReadPrimitiveDataImpl<uint64>())
        .def("read_primitive_data_float", ReadPrimitiveDataImpl<float>())
        .def("read_primitive_data_double", ReadPrimitiveDataImpl<double>())
        .def("read_primitive_data_bool", ReadPrimitiveDataImpl<bool>())
        .def("read_primitive_data_String", ReadPrimitiveDataImpl<String>())
        .def("read_primitive_data_Float3", ReadPrimitiveDataImpl<Float3>())
        .def("read_primitive_data_Double3", ReadPrimitiveDataImpl<Double3>())
        .def("read_primitive_data_Vec3", ReadPrimitiveDataImpl<Vec3>())
        .def("read_primitive_data_DVec3", ReadPrimitiveDataImpl<DVec3>())
        .def("read_primitive_data_Vec4", ReadPrimitiveDataImpl<Vec4>())
        .def("read_primitive_data_Quat", ReadPrimitiveDataImpl<Quat>())
        .def("read_primitive_data_Mat44", ReadPrimitiveDataImpl<Mat44>())
        .def("read_primitive_data_DMat44", ReadPrimitiveDataImpl<DMat44>());
}