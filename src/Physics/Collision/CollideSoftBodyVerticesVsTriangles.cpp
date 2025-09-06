#include "Common.h"
#include <Jolt/Physics/Collision/Shape/ScaleHelpers.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVerticesVsTriangles.h>

void BindCollideSoftBodyVerticesVsTriangles(nb::module_ &m) {
    nb::class_<CollideSoftBodyVerticesVsTriangles> collideSoftBodyVerticesVsTrianglesCls(m, "CollideSoftBodyVerticesVsTriangles",
        "Collision detection helper that collides soft body vertices vs triangles");
    collideSoftBodyVerticesVsTrianglesCls
        .def(nb::init<Mat44Arg, Vec3Arg>(), "center_of_mass_transform"_a, "scale"_a)
        .def("start_vertex", &CollideSoftBodyVerticesVsTriangles::StartVertex, "vertex"_a)
        .def("process_triangle", &CollideSoftBodyVerticesVsTriangles::ProcessTriangle, "v0"_a, "v1"_a, "v2"_a)
        .def("finish_vertex", &CollideSoftBodyVerticesVsTriangles::FinishVertex, "vertex"_a, "colliding_shape_index"_a)
        .def_rw("transform", &CollideSoftBodyVerticesVsTriangles::mTransform)
        .def_rw("inv_transform", &CollideSoftBodyVerticesVsTriangles::mInvTransform)
        .def_rw("local_position", &CollideSoftBodyVerticesVsTriangles::mLocalPosition)
        .def_rw("v0", &CollideSoftBodyVerticesVsTriangles::mV0)
        .def_rw("v1", &CollideSoftBodyVerticesVsTriangles::mV1)
        .def_rw("v2", &CollideSoftBodyVerticesVsTriangles::mV2)
        .def_rw("closest_point", &CollideSoftBodyVerticesVsTriangles::mClosestPoint)
        .def_rw("normal_sign", &CollideSoftBodyVerticesVsTriangles::mNormalSign)
        .def_rw("closest_distance_sq", &CollideSoftBodyVerticesVsTriangles::mClosestDistanceSq)
        .def_rw("set", &CollideSoftBodyVerticesVsTriangles::mSet);
}