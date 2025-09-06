#include "Common.h"
#include <Jolt/Core/RTTI.h>
#include <nanobind/operators.h>

void BindRTTI(nb::module_ &m) {

    using pCreateObjectFunction = void *(*)();

    using pDestructObjectFunction = void (*)(void *inObject);

    using pCreateRTTIFunction = void (*)(RTTI &inRTTI);

    nb::class_<RTTI>(m, "RTTI")
        // TODO:
        // .def(nb::init<const char *, int, pCreateObjectFunction, pDestructObjectFunction>(),"name"_a,"size"_a,"create_object"_a,"destroy_object"_a)
        // .def(nb::init<const char *, int, pCreateObjectFunction, pDestructObjectFunction, pCreateRTTIFunction>(),"name"_a,"size"_a,"create_object"_a,"destroy_object"_a,"create_rtti"_a)

        .def("get_name", &RTTI::GetName, "Properties")
        .def("set_name", &RTTI::SetName, "name"_a)
        .def("get_size", &RTTI::GetSize)
        .def("is_abstract", &RTTI::IsAbstract)
        .def("get_base_class_count", &RTTI::GetBaseClassCount)
        .def("get_base_class", &RTTI::GetBaseClass, "index"_a)
        .def("get_hash", &RTTI::GetHash)
        .def("create_object", &RTTI::CreateObject, "Create an object of this type (returns nullptr if the object is abstract)")
        .def("destruct_object", &RTTI::DestructObject, "Destruct object of this type (does nothing if the object is abstract)")
        .def("add_base_class", &RTTI::AddBaseClass, "Add base class")

        .def(nb::self == nb::self, "rhs"_a, "Equality operators")
        .def(nb::self != nb::self, "rhs"_a, "Equality operators")

        .def("is_kind_of", &RTTI::IsKindOf, "Test if this class is derived from class of type inRTTI")
        // .def("cast_to", &RTTI::CastTo, "Cast inObject of this type to object of type inRTTI, returns nullptr if the cast is unsuccessful")

        .def("add_attribute", &RTTI::AddAttribute, "attribute"_a, "Attribute access")
        .def("get_attribute_count", &RTTI::GetAttributeCount)
        .def("get_attribute", &RTTI::GetAttribute, "index"_a);
}