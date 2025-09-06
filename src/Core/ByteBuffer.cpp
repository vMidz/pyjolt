#include "Common.h"
#include <Jolt/Core/ByteBuffer.h>

using ByteBufferVector = Array<uint8, STLAlignedAllocator<uint8, JPH_CACHE_LINE_SIZE>>;

template <typename T>
static void BindInst(nanobind::class_<ByteBuffer, ByteBufferVector> &cls, std::string name) {
    std::string allocate = "allocate" + name;
    std::string append = "AppendVector" + name;
    std::string get = "get" + name;
    cls.def(allocate.c_str(), &ByteBuffer::Allocate<T>, "size"_a = 1, "Allocate block of data of inSize elements and return the pointer")
        .def(append.c_str(), &ByteBuffer::AppendVector<T>, "data"_a, "Append inData to the buffer")
        .def(get.c_str(), [](ByteBuffer &self, size_t pos) {
            return self.Get<T>(pos);
        }, "position"_a, "Get object at inPosition (an offset in bytes)");
}

void BindByteBuffer(nb::module_ &m) {
    nb::class_<ByteBufferVector>(m, "ByteBufferVector", "Underlying data type for ByteBuffer");

    nb::class_<ByteBuffer, ByteBufferVector> cls(m, "ByteBuffer", "Simple byte buffer, aligned to a cache line");
    cls
        .def("align", &ByteBuffer::Align, "size"_a, "Align the size to a multiple of inSize, returns the length after alignment");

    BindInst<int>(cls, std::string("_int"));
    BindInst<float>(cls, std::string("_float"));
}