#include "Common.h"
#include <Jolt/Physics/SoftBody/SoftBodyShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

void BindSoftBodyShape(nb::module_ &m) {
    nb::class_<SoftBodyShape, Shape> softBodyShapeCls(m, "SoftBodyShape",
        "Shape used exclusively for soft bodies. Adds the ability to perform collision checks against soft bodies.",
        nb::is_final());
    softBodyShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def("get_sub_shape_id_bits", &SoftBodyShape::GetSubShapeIDBits,
            "Determine amount of bits needed to encode sub shape id")
        .def("get_face_index", &SoftBodyShape::GetFaceIndex, "sub_shape_id"_a,
            "Convert a sub shape ID back to a face index")
        .def("must_be_static", &SoftBodyShape::MustBeStatic)
        .def("get_center_of_mass", &SoftBodyShape::GetCenterOfMass)
        .def("get_local_bounds", &SoftBodyShape::GetLocalBounds)
        .def("get_sub_shape_id_bits_recursive", &SoftBodyShape::GetSubShapeIDBitsRecursive)
        .def("get_inner_radius", &SoftBodyShape::GetInnerRadius)
        .def("get_mass_properties", &SoftBodyShape::GetMassProperties)
        .def("get_material", &SoftBodyShape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference)
        .def("get_surface_normal", &SoftBodyShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &SoftBodyShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_submerged_volume", &SoftBodyShape::GetSubmergedVolume, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "total_volume"_a, "submerged_volume"_a, "center_of_buoyancy"_a, "base_offset"_a)
        .def("draw", &SoftBodyShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&SoftBodyShape::CastRay, nb::const_),
            "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&SoftBodyShape::CastRay, nb::const_),
            "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &SoftBodyShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &SoftBodyShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &SoftBodyShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &SoftBodyShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("get_stats", &SoftBodyShape::GetStats)
        .def("get_volume", &SoftBodyShape::GetVolume)
        .def_static("register", &SoftBodyShape::sRegister);
}