#include "Common.h"
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>

void BindObjectLayerPairFilterTable(nb::module_ &m) {
    nb::class_<ObjectLayerPairFilterTable, ObjectLayerPairFilter> objectLayerPairFilterTableCls(m, "ObjectLayerPairFilterTable",
        "Filter class to test if two objects can collide based on their object layer. Used while finding collision pairs.\n"
        "This implementation uses a table to determine if two layers can collide.");
    objectLayerPairFilterTableCls
        .def(nb::init<uint>(), "num_object_layers"_a,
            "Constructs the table with inNumObjectLayers Layers, initially all layer pairs are disabled")
        .def("get_num_object_layers", &ObjectLayerPairFilterTable::GetNumObjectLayers,
            "Get the number of object layers")
        .def("disable_collision", &ObjectLayerPairFilterTable::DisableCollision, "layer1"_a, "layer2"_a,
            "Disable collision between two object layers")
        .def("enable_collision", &ObjectLayerPairFilterTable::EnableCollision, "layer1"_a, "layer2"_a,
            "Enable collision between two object layers")
        .def("should_collide", &ObjectLayerPairFilterTable::ShouldCollide, "object1"_a, "object2"_a,
            "Returns true if two layers can collide");
}