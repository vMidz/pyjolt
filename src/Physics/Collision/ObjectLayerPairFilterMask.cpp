#include "Common.h"
#include <Jolt/Physics/Collision/ObjectLayerPairFilterMask.h>

void BindObjectLayerPairFilterMask(nb::module_ &m) {
    nb::class_<ObjectLayerPairFilterMask, ObjectLayerPairFilter> objectLayerPairFilterMaskCls(m, "ObjectLayerPairFilterMask",
        "Filter class to test if two objects can collide based on their object layer. Used while finding collision pairs.\n"
        "Uses group bits and mask bits. Two layers can collide if Object1.Group & Object2.Mask is non-zero and Object2.Group & Object1.Mask is non-zero.\n"
        "The behavior is similar to that in e.g. Bullet.\n"
        "This implementation works together with BroadPhaseLayerInterfaceMask and ObjectVsBroadPhaseLayerFilterMask");
    objectLayerPairFilterMaskCls
        .def_static("get_object_layer", &ObjectLayerPairFilterMask::sGetObjectLayer, "group"_a, "mask"_a = ObjectLayerPairFilterMask::cMask,
            "Construct an ObjectLayer from a group and mask bits")
        .def_static("get_group", &ObjectLayerPairFilterMask::sGetGroup, "object_layer"_a,
            "Get the group bits from an ObjectLayer")
        .def_static("get_mask", &ObjectLayerPairFilterMask::sGetMask, "object_layer"_a,
            "Get the mask bits from an ObjectLayer")
        .def("should_collide", &ObjectLayerPairFilterMask::ShouldCollide, "object1"_a, "object2"_a,
            "Returns true if two layers can collide")
        .def_ro_static("C_NUM_BITS", &ObjectLayerPairFilterMask::cNumBits,
            "Number of bits for the group and mask bits")
        .def_ro_static("C_MASK", &ObjectLayerPairFilterMask::cMask);
}
