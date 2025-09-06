#include "Common.h"
#include <Jolt/Geometry/AABox.h>

#include <nanobind/stl/vector.h>
#include <nanobind/operators.h>

void BindAABox(nb::module_ &m) {
    nb::class_<AABox>(m, "AABox", "Axis aligned box")
        // Constructors
        .def(nb::init<>())
        .def(nb::init<const Vec3 &, const Vec3 &>(), "min"_a, "max"_a)
        .def(nb::init<const DVec3 &, const DVec3 &>(), "min"_a, "max"_a)
        .def(nb::init<const Vec3 &, float>(), "center"_a, "radius"_a)

        .def_static("from_two_points", &AABox::sFromTwoPoints, "point_1"_a, "point_2"_a, "Create box from 2 points")
        .def_static("biggest", &AABox::sBiggest, "Get bounding box of size 2 * FLT_MAX")

        .def_rw("min", &AABox::mMin)
        .def_rw("max", &AABox::mMax)

        .def("set_empty", &AABox::SetEmpty, "Reset the bounding box to an empty bounding box")
        .def("is_valid", &AABox::IsValid, "Check if the bounding box is valid (max >= min)")
        .def("encapsulate", nb::overload_cast<Vec3Arg>(&AABox::Encapsulate), "pos"_a, "Encapsulate point in bounding box")
        .def("encapsulate", nb::overload_cast<const AABox &>(&AABox::Encapsulate), "rhs"_a, "Encapsulate bounding box in bounding box")
        .def("encapsulate", nb::overload_cast<const Triangle &>(&AABox::Encapsulate), "rhs"_a, "Encapsulate triangle in bounding box")
        .def("encapsulate", nb::overload_cast<const VertexList &, const IndexedTriangle &>(&AABox::Encapsulate),
            "vertices"_a, "triangle"_a,"Encapsulate triangle in bounding box")
        .def("intersect", &AABox::Intersect, "other"_a, "Intersect this bounding box with inOther, returns the intersection")
        .def("ensure_minimal_edge_length", &AABox::EnsureMinimalEdgeLength,
            "min_edge_length"_a, "Make sure that each edge of the bounding box has a minimal length")
        .def("expand_by", &AABox::ExpandBy, "vector"_a, "Widen the box on both sides by inVector")
        .def("get_center", &AABox::GetCenter, "Get center of bounding box")
        .def("get_extent", &AABox::GetExtent, "Get extent of bounding box (half of the size)")
        .def("get_size", &AABox::GetSize, "Get size of bounding box")
        .def("get_surface_area", &AABox::GetSurfaceArea, "Get surface area of bounding box")
        .def("get_volume", &AABox::GetVolume, "Get volume of bounding box")
        .def("contains", nb::overload_cast<const AABox &>(&AABox::Contains, nb::const_), "other"_a, "Check if this box contains another box")
        .def("contains", nb::overload_cast<Vec3Arg>(&AABox::Contains, nb::const_), "other"_a, "Check if this box contains a point")
        .def("contains", nb::overload_cast<DVec3Arg>(&AABox::Contains, nb::const_), "other"_a, "Check if this box contains a point")
        .def("overlaps", nb::overload_cast<const AABox &>(&AABox::Overlaps, nb::const_),
            "other"_a, "Check if this box overlaps with another box")
        .def("overlaps", nb::overload_cast<const Plane &>(&AABox::Overlaps, nb::const_),
            "plane"_a, "Check if this box overlaps with a plane")
        .def("translate", nb::overload_cast<Vec3Arg>(&AABox::Translate), "translation"_a, "Translate bounding box")
        .def("translate", nb::overload_cast<DVec3Arg>(&AABox::Translate), "translation"_a, "Translate bounding box")
        .def("transformed", nb::overload_cast<Mat44Arg>(&AABox::Transformed, nb::const_), "matrix"_a, "Transform bounding box")
        .def("transformed", nb::overload_cast<DMat44Arg>(&AABox::Transformed, nb::const_), "matrix"_a, "Transform bounding box")
        .def("scaled", &AABox::Scaled, "scale"_a, "Scale this bounding box, can handle non-uniform and negative scaling")
        .def("get_support", &AABox::GetSupport, "direction"_a, "Calculate the support vector for this convex shape.")
        .def("get_supporting_face", [](AABox &self, Vec3 inDirection) {
            Array<Vec3> out;
            self.GetSupportingFace(inDirection, out);
            return out;
        }, "direction"_a, "Get the vertices of the face that faces inDirection the most")
        .def("get_closest_point", &AABox::GetClosestPoint, "point"_a, "Get the closest point on or in this box to inPoint")
        .def("get_sq_distance_to", &AABox::GetSqDistanceTo,
            "point"_a, "Get the squared distance between inPoint and this box (will be 0 if in Point is inside the box)")

        // Operators
        .def(nb::self == nb::self, "rhs"_a, "Comparison operators")
        .def(nb::self != nb::self, "rhs"_a, "Comparison operators")

        // Python-specific
        .def("__repr__", [](const AABox &self) {
            const Vec3 &min = self.mMin;
            const Vec3 &max = self.mMax;
            return nb::str("AABox(min=({}, {}, {}), max=({}, {}, {}))").format(min.GetX(), min.GetY(), min.GetZ(), max.GetX(), max.GetY(), max.GetZ());
        });
}