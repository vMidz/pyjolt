#include "Common.h"
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>
#include <Jolt/ObjectStream/ObjectStreamIn.h>
#include <Jolt/Core/StreamUtils.h>

#include <nanobind/trampoline.h>

class PyPhysicsMaterial : public PhysicsMaterial {
  public:
    NB_TRAMPOLINE(PhysicsMaterial, 4);

    const char *GetDebugName() const override {
        NB_OVERRIDE_NAME("get_debug_name", GetDebugName);
    }

    Color GetDebugColor() const override {
        NB_OVERRIDE_NAME("get_debug_color", GetDebugColor);
    }

    void SaveBinaryState(StreamOut &inStream) const override {
        NB_OVERRIDE_NAME("save_binary_state", SaveBinaryState, inStream);
    }

    void RestoreBinaryState(StreamIn &inStream) override {
        NB_OVERRIDE_NAME("restore_binary_state", RestoreBinaryState, inStream);
    }
};

void BindPhysicsMaterial(nb::module_ &m) {
    nb::class_<PhysicsMaterial, RefTarget<PhysicsMaterial>> physicsMaterialCls(m, "PhysicsMaterial",
        nb::intrusive_ptr<PhysicsMaterial>([](PhysicsMaterial *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }),
        "This structure describes the surface of (part of) a shape. You should inherit from it to define additional\n"
        "information that is interesting for the simulation. The 2 materials involved in a contact could be used\n"
        "to decide which sound or particle effects to play.\n"
        "If you inherit from this material, don't forget to create a suitable default material in sDefault");
    physicsMaterialCls
        .def(nb::init<>())
        .def("get_debug_name", &PhysicsMaterial::GetDebugName, nb::rv_policy::reference)
        .def("get_debug_color", &PhysicsMaterial::GetDebugColor)
        .def("save_binary_state", &PhysicsMaterial::SaveBinaryState, "stream"_a,
            "Saves the contents of the material in binary form to inStream.")
        .def_static("restore_from_binary_state", &PhysicsMaterial::sRestoreFromBinaryState, "stream"_a,
            "Creates a PhysicsMaterial of the correct type and restores its contents from the binary stream inStream.")
        .def_rw_static("default", &PhysicsMaterial::sDefault,
            "Default material that is used when a shape has no materials defined");
}
