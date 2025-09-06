#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.h>

void BindObjectVsBroadPhaseLayerFilterMask(nb::module_ &m) {
    nb::class_<ObjectVsBroadPhaseLayerFilterMask, ObjectVsBroadPhaseLayerFilter> objectVsBroadPhaseLayerFilterMaskCls(m, "ObjectVsBroadPhaseLayerFilterMask",
        "Class that determines if an object layer can collide with a broadphase layer.\n"
        "This implementation works together with BroadPhaseLayerInterfaceMask and ObjectLayerPairFilterMask");
    objectVsBroadPhaseLayerFilterMaskCls
        .def(nb::init<const BroadPhaseLayerInterfaceMask &>(), "broad_phase_layer_interface"_a,
            "Constructor")
        .def("should_collide", &ObjectVsBroadPhaseLayerFilterMask::ShouldCollide, "layer1"_a, "layer2"_a,
            "Returns true if an object layer should collide with a broadphase layer");

}