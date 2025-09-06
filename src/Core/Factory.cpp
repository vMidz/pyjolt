#include "Common.h"
#include <Jolt/Core/Factory.h>
#include <nanobind/stl/vector.h>

void BindFactory(nb::module_ &m) {
    nb::class_<Factory>(m, "Factory")
        .def("create_object", [](Factory &self, const char *name) -> nb::object {
            void *obj = self.CreateObject(name);
            if (obj == nullptr)
                return nb::none();

            return nb::capsule(obj, "void_ptr");
        }, "name"_a, "Create an object by name")

        .def("find_by_name", [](Factory &self, const char *name) -> nb::object {
            const RTTI *rtti = self.Find(name);
            if (rtti == nullptr) {
                return nb::none();
            }
            return nb::cast(rtti);
        }, "name"_a, "Find type info for a specific class by name")

        .def("find_by_hash", [](Factory &self, uint32_t hash) -> nb::object {
            const RTTI *rtti = self.Find(hash);
            if (rtti == nullptr) {
                return nb::none();
            }
            return nb::cast(rtti);
        }, "hash"_a, "Find type info for a specific class by hash")

        .def("register_type", [](Factory &self, const RTTI *rtti) -> bool {
            return self.Register(rtti);
        }, "rtti"_a, "Register an object with the factory")

        .def("register_types", [](Factory &self, const std::vector<const RTTI *> &rttis) -> bool {
            // Convert vector to array pointer for the C++ function
            return self.Register((const RTTI **)rttis.data(), (uint)rttis.size());
        }, "rttis"_a, "Register a list of objects with the factory")

        .def("clear", &Factory::Clear, "Unregisters all types")

        .def("get_all_classes", [](Factory &self) -> std::vector<const RTTI *> {
            Array<const RTTI *> classes = self.GetAllClasses();
            return std::vector<const RTTI *>(classes.begin(), classes.end());
        }, "Get all registered classes")

        // Static instance - using a function to return the singleton
        .def_static("get_instance", []() -> Factory * {
            return Factory::sInstance;
        }, "Get the singleton factory instance");

    m.def("new_factory", []() {
        if (Factory::sInstance)
            delete Factory::sInstance;

        Factory::sInstance = new Factory();
    }, "This class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.");

    m.def("delete_factory", []() {
        if (Factory::sInstance)
            delete Factory::sInstance;

        Factory::sInstance = nullptr;
    });
}