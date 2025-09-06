#include "Common.h"
#include <Jolt/Geometry/Plane.h>

void BindPlane(nb::module_ &m) {
    nb::class_<Plane>(m, "Plane", "An infinite plane described by the formula X . Normal + Constant = 0.")
        .def(nb::init<>(), "Default Constructor")
        .def(nb::init<Vec4Arg>(), "normal_and_constant"_a)
        .def(nb::init<Vec3Arg, float>(), "normal"_a, "constant"_a)
        .def_static("from_point_and_normal", nb::overload_cast<Vec3Arg, Vec3Arg>(Plane::sFromPointAndNormal),
            "point"_a, "normal"_a, "Create from point and normal")
        .def_static("from_point_and_normal", nb::overload_cast<DVec3Arg, Vec3Arg>(&Plane::sFromPointAndNormal),
            "point"_a, "normal"_a, "Create from point and normal, double precision version that more accurately calculates the plane constant")
        .def_static("from_points_ccw", &Plane::sFromPointsCCW,
            "v_1"_a, "v_2"_a, "v_3"_a, "Create from 3 counter clockwise points")

        .def_prop_rw("normal", &Plane::GetNormal, &Plane::SetNormal, "normal"_a, "Get/set normal")
        .def_prop_rw("constant", &Plane::GetConstant, &Plane::SetConstant, "constant"_a, "Get/set constant")

        .def("offset", &Plane::Offset, "distance"_a, "Offset the plane (positive value means move it in the direction of the plane normal)")
        .def("get_transformed", &Plane::GetTransformed, "transform"_a, "Transform the plane by a matrix")
        .def("scaled", &Plane::Scaled, "scale"_a, "Scale the plane, can handle non-uniform and negative scaling")
        .def("signed_distance", &Plane::SignedDistance, "point"_a, "Distance point to plane")
        .def("project_point_on_plane", &Plane::ProjectPointOnPlane, "point"_a, "Project inPoint onto the plane")
        .def_static("intersect_planes", [](const Plane &inP1, const Plane &inP2, const Plane &inP3) {
            Vec3 outPoint;
            Plane::sIntersectPlanes(inP1, inP2, inP3, outPoint);
        }, "p1"_a, "p2"_a, "p3"_a, "Returns intersection point between 3 planes")
        .def("scaled", &Plane::Scaled, "scale"_a, "scale the plane, can handle non-uniform and negative scaling");
}