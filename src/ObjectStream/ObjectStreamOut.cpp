#include "Common.h"
#include <Jolt/ObjectStream/ObjectStreamBinaryOut.h>
#include <Jolt/Physics/PhysicsScene.h>

template <typename T>
void BindRefInst(nb::class_<ObjectStreamOut, IObjectStreamOut> &cls, std::string name) {
    cls.def_static(("write_object_ref_" + name).c_str(), [](const char *inFileName, ObjectStream::EStreamType inType) {
        Ref<T> out;
        auto r = ObjectStreamOut::sWriteObject(inFileName, inType, *out);
        return nb::make_tuple(r, out);
    }, "file_name"_a, "type"_a, "Main function to write an object to a file");

    cls.def_static(("write_object_" + name).c_str(), [](const char *inFileName, ObjectStream::EStreamType inType) {
        T *out;
        auto r = ObjectStreamOut::sWriteObject(inFileName, inType, *out);
        return nb::make_tuple(r, nb::capsule(out));
    }, "file_name"_a, "type"_a, "Main function to write an object to a file");
}

void BindObjectStreamOut(nb::module_ &m) {
    nb::class_<ObjectStreamOut, IObjectStreamOut> cls(m, "ObjectStreamOut", "ObjectStreamOut contains all logic for writing an object to disk. It is the base\n"
                                                                            "class for the text and binary output streams (ObjectStreamTextOut and ObjectStreamBinaryOut).");

    // BindRefInst<PhysicsScene>(cls, "physics_scene");
}
