#include "Common.h"
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>
#include <Jolt/Core/StreamOut.h>
#include <nanobind/stl/string.h>
#include <nanobind/trampoline.h>

class PyPhysicsMaterialSimple : public PhysicsMaterialSimple
{
    NB_TRAMPOLINE(PhysicsMaterialSimple, 3);

    ~PyPhysicsMaterialSimple() override {
    }

    virtual const char * GetDebugName() const override {
        NB_OVERRIDE_NAME(
            "get_debug_name",
            GetDebugName);
    }

    virtual Color GetDebugColor() const override {
        NB_OVERRIDE_NAME(
            "get_debug_color",
            GetDebugColor);
    }

    virtual void SaveBinaryState(StreamOut &inStream) const override {
        NB_OVERRIDE_NAME(
            "save_binary_state",
            SaveBinaryState,
            inStream);
    }
};

void BindPhysicsMaterialSimple(nb::module_ &m) {
    nb::class_<PhysicsMaterialSimple, PhysicsMaterial> physicsMaterialSimpleCls(m, "PhysicsMaterialSimple",
        "Sample implementation of PhysicsMaterial that just holds the needed properties directly");
    physicsMaterialSimpleCls
        .def(nb::init<>(),
            "Constructor")
        .def("__init__", [](PhysicsMaterialSimple &self, const char* inName, ColorArg inColor){
            new (&self) PhysicsMaterialSimple(inName, inColor);
        }, "name"_a, "color"_a)
        .def("get_debug_name", &PhysicsMaterialSimple::GetDebugName, nb::rv_policy::reference)
        .def("get_debug_color", &PhysicsMaterialSimple::GetDebugColor)
        .def("save_binary_state", &PhysicsMaterialSimple::SaveBinaryState, "stream"_a);
}