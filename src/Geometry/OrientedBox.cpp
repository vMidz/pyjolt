#include "Common.h"
#include <Jolt/Geometry/OrientedBox.h>

void BindOrientedBox(nb::module_ &m) {
    nb::class_<OrientedBox>(m, "OrientedBox", "Oriented box")
        .def(nb::init<>(), "Default Constructor")
        .def(nb::init<Mat44Arg, Vec3Arg>(), "orientation"_a, "in_half_extents"_a)
        .def(nb::init<Mat44Arg, const AABox>(), "orientation"_a, "half_extent"_a,
            "Construct from axis aligned box and transform. Only works for rotation/translation matrix (no scaling / shearing).")

        .def("overlaps", nb::overload_cast<const AABox &, float>(&OrientedBox::Overlaps, nb::const_),
            "box"_a, "epsilon"_a = 1.0e-6f, "Test if oriented box overlaps with axis aligned box each other")

        .def("overlaps", nb::overload_cast<const OrientedBox &, float>(&OrientedBox::Overlaps, nb::const_),
            "box"_a, "epsilon"_a = 1.0e-6f, "Test if two oriented boxes overlap each other")

        .def_rw("orientation", &OrientedBox::mOrientation, "Transform that positions and rotates the local space axis aligned box into world space")
        .def_rw("half_extents", &OrientedBox::mHalfExtents, "Half extents (half the size of the edge) of the local space axis aligned box");
}