#include "Common.h"
#include <Jolt/RegisterTypes.h>

void BindRegisterTypes(nb::module_ &m) {
    m.def("verify_jolt_version_id_internal", &VerifyJoltVersionIDInternal,
        "Internal helper function");

    m.def("verify_jolt_version_id", &VerifyJoltVersionID,
        "This function can be used to verify the library ABI is compatible with your application.\n"
        "Use it in this way: assert verify_jolt_version_id() Returns false if the library used is not compatible with your app.");

    m.def("register_types_internal", &RegisterTypesInternal,
        "Internal helper function");

    m.def("register_types", &RegisterTypes,
        "Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.\n"
        "If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch\n"
        "before calling this function. If you implement your own default material (PhysicsMaterial::sDefault) make\n"
        "sure to initialize it before this function or else this function will create one for you.");

    m.def("unregister_types", &UnregisterTypes,
        "Unregisters all types with the factory and cleans up the default material");
}