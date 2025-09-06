#include "Common.h"
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

void BindConvexHullShape(nb::module_ &m) {
    nb::class_<ConvexHullShapeSettings, ConvexShapeSettings> convexHullShapeSettingsCls(m, "ConvexHullShapeSettings",
        "Class that constructs a ConvexHullShape",
        nb::is_final());
    convexHullShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<const Vec3 *, int, float, const PhysicsMaterial *>(), "points"_a, "num_points"_a, "max_convex_radius"_a = cDefaultConvexRadius, "material"_a = nullptr,
            "Create a convex hull from inPoints and maximum convex radius inMaxConvexRadius, the radius is automatically lowered if the hull requires it.\n"
            "(internally this will be subtracted so the total size will not grow with the convex radius).")
        .def(nb::init<const Array<Vec3> &, float, const PhysicsMaterial *>(), "points"_a, "convex_radius"_a = cDefaultConvexRadius, "material"_a = nullptr)
        .def("create", &ConvexHullShapeSettings::Create)
        .def_rw("points", &ConvexHullShapeSettings::mPoints,
            "Points to create the hull from")
        .def_rw("max_convex_radius", &ConvexHullShapeSettings::mMaxConvexRadius,
            "Convex radius as supplied by the constructor. Note that during hull creation the convex radius can be made smaller if the value is too big for the hull.")
        .def_rw("max_error_convex_radius", &ConvexHullShapeSettings::mMaxErrorConvexRadius,
            "Maximum distance between the shrunk hull + convex radius and the actual hull.")
        .def_rw("hull_tolerance", &ConvexHullShapeSettings::mHullTolerance,
            "Points are allowed this far outside of the hull (increasing this yields a hull with less vertices). Note that the actual used value can be larger if the points of the hull are far apart.");

    nb::class_<ConvexHullShape, ConvexShape> convexHullShapeCls(m, "ConvexHullShape",
        "A convex hull",
        nb::is_final());
    convexHullShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const ConvexHullShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def("get_center_of_mass", &ConvexHullShape::GetCenterOfMass)
        .def("get_local_bounds", &ConvexHullShape::GetLocalBounds)
        .def("get_inner_radius", &ConvexHullShape::GetInnerRadius)
        .def("get_mass_properties", &ConvexHullShape::GetMassProperties)
        .def("get_surface_normal", &ConvexHullShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &ConvexHullShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &ConvexHullShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("get_submerged_volume", [](ConvexHullShape &self,
                                        Mat44Arg inCenterOfMassTransform,
                                        Vec3Arg inScale,
                                        const Plane &inSurface,
                                        RVec3Arg inBaseOffset){

            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume,outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a, "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("draw", &ConvexHullShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("draw_shrunk_shape", &ConvexHullShape::DrawShrunkShape, "renderer"_a, "center_of_mass_transform"_a, "scale"_a,
            "Debugging helper draw function that draws how all points are moved when a shape is shrunk by the convex radius")
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&ConvexHullShape::CastRay, nb::const_),
            "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&ConvexHullShape::CastRay, nb::const_),
            "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &ConvexHullShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &ConvexHullShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &ConvexHullShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &ConvexHullShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &ConvexHullShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &ConvexHullShape::GetStats)
        .def("get_volume", &ConvexHullShape::GetVolume)
        .def("get_convex_radius", &ConvexHullShape::GetConvexRadius,
            "Get the convex radius of this convex hull")
        .def("get_planes", &ConvexHullShape::GetPlanes,
            "Get the planes of this convex hull")
        .def("get_num_points", &ConvexHullShape::GetNumPoints,
            "Get the number of vertices in this convex hull")
        .def("get_point", &ConvexHullShape::GetPoint, "index"_a,
            "Get a vertex of this convex hull relative to the center of mass")
        .def("get_num_faces", &ConvexHullShape::GetNumFaces,
            "Get the number of faces in this convex hull")
        .def("get_num_vertices_in_face", &ConvexHullShape::GetNumVerticesInFace, "face_index"_a,
            "Get the number of vertices in a face")
        .def("get_face_vertices", &ConvexHullShape::GetFaceVertices, "face_index"_a, "max_vertices"_a, "vertices"_a,
            "Get the vertices indices of a face.\n"
            "Args:\n"
            "    face_index (int): Index of the face.\n"
            "    max_vertices (int): Maximum number of vertices to return.\n"
            "    vertices (uint*): Array of vertices indices, must be at least inMaxVertices in size, the vertices are returned in counter clockwise order and the positions can be obtained using GetPoint(index).\n"
            "Returns:\n"
            "    uint: Number of vertices in face, if this is bigger than inMaxVertices, not all vertices were retrieved.")
        .def_static("register", &ConvexHullShape::sRegister)
        .def_ro_static("C_MAX_POINTS_IN_HULL", &ConvexHullShape::cMaxPointsInHull,
            "Maximum amount of points supported in a convex hull. Note that while constructing a hull, interior points are discarded so you can provide more points.\n"
            "The ConvexHullShapeSettings::Create function will return an error when too many points are provided.")
        .def_rw_static("draw_face_outlines", &ConvexHullShape::sDrawFaceOutlines,
            "Draw the outlines of the faces of the convex hull when drawing the shape");
}