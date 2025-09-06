#include "Common.h"
#include <Jolt/ObjectStream/ObjectStreamIn.h>
#include <Jolt/Physics/PhysicsScene.h>

template <typename T>
void BindRefInst(nb::class_<ObjectStreamIn, IObjectStreamIn> &cls, std::string name) {
    cls.def_static(("read_object_ref_" + name).c_str(), [](const char *inFileName) {
        Ref<T> out;
        auto r = ObjectStreamIn::sReadObject(inFileName, out);
        return nb::make_tuple(r, out);
    }, "file_name"_a, "Main function to read an object from a stream");

    cls.def_static(("read_object_" + name).c_str(), [](const char *inFileName) {
        T *out;
        auto r = ObjectStreamIn::sReadObject(inFileName, out);
        return nb::make_tuple(r, nb::capsule(out));
    },"file_name"_a, "Main function to read an object from a stream");
}

void BindObjectStreamIn(nb::module_ &m) {
    nb::class_<ObjectStreamIn, IObjectStreamIn> cls(m, "ObjectStreamIn", "ObjectStreamIn contains all logic for reading an object from disk. It is the base\n"
                                                                         "class for the text and binary input streams (ObjectStreamTextIn and ObjectStreamBinaryIn).");

    ;
}