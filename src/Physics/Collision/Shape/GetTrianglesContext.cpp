#include "Common.h"
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/Shape/ScaleHelpers.h>
#include <Jolt/Physics/Collision/Shape/GetTrianglesContext.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/TransformedShape.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>

void BindGetTrianglesContext(nb::module_ &m) {
    nb::class_<GetTrianglesContextVertexList> getTrianglesContextVertexListCls(m, "GetTrianglesContextVertexList",
        "Implementation of GetTrianglesStart/Next that uses a fixed list of vertices for the triangles. These are transformed into world space when getting the triangles.");
    getTrianglesContextVertexListCls
        .def(nb::init<Vec3Arg, QuatArg, Vec3Arg, Mat44Arg, const Vec3 *, size_t , const PhysicsMaterial *>(), "position_com"_a, "rotation"_a, "scale"_a, "local_transform"_a, "triangle_vertices"_a, "num_triangle_vertices"_a, "material"_a,
        "Constructor, to be called in GetTrianglesStart") // TODO:
        // .def("get_triangles_next", &GetTrianglesContextVertexList::GetTrianglesNext, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a,
        //     "@see Shape::GetTrianglesNext")
        // .def_static("create_half_unit_sphere_top", &GetTrianglesContextVertexList::sCreateHalfUnitSphereTop, "vertices"_a, "detail_level"_a,
            // "Helper function that creates a vertex list of a half unit sphere (top part)")
        // .def_static("create_half_unit_sphere_bottom", &GetTrianglesContextVertexList::sCreateHalfUnitSphereBottom, "vertices"_a, "detail_level"_a,
            // "Helper function that creates a vertex list of a half unit sphere (bottom part)")
        // .def_static("create_unit_open_cylinder", &GetTrianglesContextVertexList::sCreateUnitOpenCylinder, "vertices"_a, "detail_level"_a,
            // "Helper function that creates an open cylinder of half height 1 and radius 1")
            ;

    nb::class_<GetTrianglesContextMultiVertexList> getTrianglesContextMultiVertexListCls(m, "GetTrianglesContextMultiVertexList",
        "Implementation of GetTrianglesStart/Next that uses a multiple fixed lists of vertices for the triangles. These are transformed into world space when getting the triangles.");
    getTrianglesContextMultiVertexListCls
        .def(nb::init<bool, const PhysicsMaterial *>(), "is_inside_out"_a, "material"_a,
            "Constructor, to be called in GetTrianglesStart")
        .def("add_part", &GetTrianglesContextMultiVertexList::AddPart, "local_to_world"_a, "triangle_vertices"_a, "num_triangle_vertices"_a,
            "Add a mesh part and its transform")
        // .def("get_triangles_next", &GetTrianglesContextMultiVertexList::GetTrianglesNext, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a,
        //     "@see Shape::GetTrianglesNext")
        ;
}