#include "Common.h"
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include <Jolt/Physics/Collision/Shape/PolyhedronSubmergedVolumeCalculator.h>

void BindPolyhedronSubmergedVolumeCalculator(nb::module_ &m) {
    nb::class_<PolyhedronSubmergedVolumeCalculator> polyhedronSubmergedVolumeCalculatorCls(m, "PolyhedronSubmergedVolumeCalculator",
        "This class calculates the intersection between a fluid surface and a polyhedron and returns the submerged volume and its center of buoyancy\n"
        "Construct this class and then one by one add all faces of the polyhedron using the AddFace function. After all faces have been added the result\n"
        "can be gotten through GetResult.");
    polyhedronSubmergedVolumeCalculatorCls
        .def(nb::init<const Mat44 &, const Vec3 *, int , int , const Plane &, PolyhedronSubmergedVolumeCalculator::Point *, RVec3>(), "transform"_a, "points"_a, "point_stride"_a, "num_points"_a, "surface"_a, "buffer"_a, "base_offset"_a,
            "Constructor.\n"
            "Args:\n"
            "    transform (Mat44): Transform to transform all incoming points with.\n"
            "    points (Vec3*): Array of points that are part of the polyhedron.\n"
            "    point_stride (int): Amount of bytes between each point (should usually be sizeof(Vec3)).\n"
            "    num_points (int): The amount of points.\n"
            "    surface (Plane): The plane that forms the fluid surface (normal should point up).\n"
            "    buffer (Point*): A temporary buffer of Point's that should have inNumPoints entries and should stay alive while this class is alive.\n"
            "    base_offset (RVec3): The offset to transform inTransform to world space (in double precision mode this can be used to shift the whole operation closer to the origin). Only used for debug drawing.")
        .def("are_all_above", &PolyhedronSubmergedVolumeCalculator::AreAllAbove,
            "Check if all points are above the surface. Should be used as early out.")
        .def("are_all_below", &PolyhedronSubmergedVolumeCalculator::AreAllBelow,
            "Check if all points are below the surface. Should be used as early out.")
        .def("get_reference_point_idx", &PolyhedronSubmergedVolumeCalculator::GetReferencePointIdx,
            "Get the lowest point of the polyhedron. Used to form the 4th vertex to make a tetrahedron out of a polyhedron face.")
        .def("add_face", &PolyhedronSubmergedVolumeCalculator::AddFace, "idx1"_a, "idx2"_a, "idx3"_a,
            "Add a polyhedron face. Supply the indices of the points that form the face (in counter clockwise order).")
        .def("get_result", &PolyhedronSubmergedVolumeCalculator::GetResult, "submerged_volume"_a, "center_of_buoyancy"_a,
            "Call after all faces have been added. Returns the submerged volume and the center of buoyancy for the submerged volume.");

    nb::class_<PolyhedronSubmergedVolumeCalculator::Point> pointCls(polyhedronSubmergedVolumeCalculatorCls, "Point",
        "A helper class that contains cached information about a polyhedron vertex");
    pointCls
        .def_rw("position", &PolyhedronSubmergedVolumeCalculator::Point::mPosition,
            "World space position of vertex")
        .def_rw("distance_to_surface", &PolyhedronSubmergedVolumeCalculator::Point::mDistanceToSurface,
            "Signed distance to the surface (> 0 is above, < 0 is below)")
        .def_rw("above_surface", &PolyhedronSubmergedVolumeCalculator::Point::mAboveSurface,
            "If the point is above the surface (mDistanceToSurface > 0)");
}