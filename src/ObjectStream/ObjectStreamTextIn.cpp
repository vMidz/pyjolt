#include "Common.h"
#include <Jolt/ObjectStream/ObjectStreamTextIn.h>

template <typename T>
auto ReadPrimitiveDataImpl() {
    return [](ObjectStreamTextIn &self) {
        T data;
        auto r = self.ReadPrimitiveData(data);
        return nb::make_tuple(r, data);
    };
}

void BindObjectStreamTextIn(nb::module_ &m) {
    nb::class_<ObjectStreamTextIn, ObjectStreamIn>(m, "ObjectStreamTextIn", "Implementation of ObjectStream text input stream.")
        .def(nb::init<istream &>(), "Constructor")

        .def("read_data_type", [](ObjectStreamTextIn &self) {
            EOSDataType out;
            auto r = self.ReadDataType(out);
            return nb::make_tuple(r, out);
        }, "Input type specific operations")

        .def("read_name", [](ObjectStreamTextIn &self) {
            String out;
            auto r = self.ReadName(out);
            return nb::make_tuple(r, out);
        })

        .def("read_identifier", [](ObjectStreamTextIn &self) {
            uint32 out;
            auto r = self.ReadIdentifier(out);
            return nb::make_tuple(r, out);
        })

        .def("read_count", [](ObjectStreamTextIn &self) {
            uint32 out;
            auto r = self.ReadCount(out);
            return nb::make_tuple(r, out);
        })

        .def("read_primitive_data", ReadPrimitiveDataImpl<uint8>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<uint16>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<int>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<uint32>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<uint64>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<float>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<double>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<bool>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<String>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<Float3>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<Double3>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<Vec3>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<DVec3>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<Vec4>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<Quat>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<Mat44>())
        .def("read_primitive_data", ReadPrimitiveDataImpl<DMat44>());
}