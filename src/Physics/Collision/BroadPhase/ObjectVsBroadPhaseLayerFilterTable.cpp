#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>

void BindObjectVsBroadPhaseLayerFilterTable(nb::module_ &m) {
    nb::class_<ObjectVsBroadPhaseLayerFilterTable, ObjectVsBroadPhaseLayerFilter> objectVsBroadPhaseLayerFilterTableCls(m, "ObjectVsBroadPhaseLayerFilterTable",
        "Class that determines if an object layer can collide with a broadphase layer.\n"
        "This implementation uses a table and constructs itself from an ObjectLayerPairFilter and a BroadPhaseLayerInterface.");
    objectVsBroadPhaseLayerFilterTableCls
        .def(nb::init<const BroadPhaseLayerInterface &, uint , const ObjectLayerPairFilter &, uint>(), "broad_phase_layer_interface"_a, "num_broad_phase_layers"_a, "object_layer_pair_filter"_a, "num_object_layers"_a,
            "Construct the table.\n"
            "Args:\n"
            "    broad_phase_layer_interface (BroadPhaseLayerInterface): The broad phase layer interface that maps object layers to broad phase layers.\n"
            "    num_broad_phase_layers (int): Number of broad phase layers.\n"
            "    object_layer_pair_filter (ObjectLayerPairFilter): The object layer pair filter that determines which object layers can collide.\n"
            "    num_object_layers (int): Number of object layers.")
        .def("should_collide", &ObjectVsBroadPhaseLayerFilterTable::ShouldCollide, "layer1"_a, "layer2"_a,
            "Returns true if an object layer should collide with a broadphase layer");
}