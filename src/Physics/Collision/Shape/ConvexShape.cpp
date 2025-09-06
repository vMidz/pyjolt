#include "Common.h"
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindConvexShape(nb::module_ &m) {
    nb::class_<ConvexShapeSettings, ShapeSettings> convexShapeSettingsCls(m, "ConvexShapeSettings",
        "Class that constructs a ConvexShape (abstract)");
    convexShapeSettingsCls
        // .def(nb::init<>(),
            // "Constructor")
        // .def(nb::init<const PhysicsMaterial *>(), "material"_a)
        .def("set_density", &ConvexShapeSettings::SetDensity, "density"_a,
            "Set the density of the object in kg / m^3")
        .def_rw("material", &ConvexShapeSettings::mMaterial,
            "Material assigned to this shape")
        .def_rw("density", &ConvexShapeSettings::mDensity,
            "Uniform density of the interior of the convex object (kg / m^3)");

    nb::class_<ConvexShape, Shape> convexShapeCls(m, "ConvexShape",
        "Base class for all convex shapes. Defines a virtual interface.");
    convexShapeCls
        // .def(nb::init<EShapeSubType>(), "sub_type"_a,
        //     "Constructor")
        // .def(nb::init<EShapeSubType, const ConvexShapeSettings &, ShapeSettings::ShapeResult &>(), "sub_type"_a, "settings"_a, "result"_a)
        // .def(nb::init<EShapeSubType, const PhysicsMaterial *>(), "sub_type"_a, "material"_a)
        .def("get_sub_shape_id_bits_recursive", &ConvexShape::GetSubShapeIDBitsRecursive)
        .def("get_material", nb::overload_cast<const SubShapeID &>(&ConvexShape::GetMaterial, nb::const_), "sub_shape_id"_a, nb::rv_policy::reference)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&ConvexShape::CastRay, nb::const_),
            "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &,
            const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&ConvexShape::CastRay, nb::const_),
            "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &ConvexShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("get_triangles_start", &ConvexShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &ConvexShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("get_submerged_volume", [](ConvexShape &self, Mat44Arg inCenterOfMassTransform,
                                        Vec3 inScale,
                                        const Plane &inSurface,
                                        Vec3 inBaseOffset){

            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
            "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("get_support_function", &ConvexShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference,
            "Returns an object that provides the GetSupport function for this shape.\n"
            "inMode determines if this support function includes or excludes the convex radius.\n"
            "of the values returned by the GetSupport function. This improves numerical accuracy of the results.\n"
            "inScale scales this shape in local space.")
        .def("set_material", &ConvexShape::SetMaterial, "material"_a,
            "Material of the shape")
        .def("get_material", nb::overload_cast<>(&ConvexShape::GetMaterial, nb::const_), nb::rv_policy::reference)
        .def("set_density", &ConvexShape::SetDensity, "density"_a,
            "Set density of the shape (kg / m^3)")
        .def("get_density", &ConvexShape::GetDensity,
            "Get density of the shape (kg / m^3)")
        .def("draw_get_support_function", &ConvexShape::DrawGetSupportFunction, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "draw_support_direction"_a)
        .def("draw_get_supporting_face", &ConvexShape::DrawGetSupportingFace, "renderer"_a, "center_of_mass_transform"_a, "scale"_a)
        .def("save_binary_state", &ConvexShape::SaveBinaryState, "stream"_a)
        .def("save_material_state", &ConvexShape::SaveMaterialState, "materials"_a)
        .def("restore_material_state", &ConvexShape::RestoreMaterialState, "materials"_a, "num_materials"_a)
        .def_static("register", &ConvexShape::sRegister);

    nb::class_<ConvexShape::Support> supportCls(convexShapeCls, "Support",
        "Function that provides an interface for GJK");
    supportCls
        .def("get_support", &ConvexShape::Support::GetSupport, "direction"_a,
            "Calculate the support vector for this convex shape (includes / excludes the convex radius depending on how this was obtained).\n"
            "Support vector is relative to the center of mass of the shape.")
        .def("get_convex_radius", &ConvexShape::Support::GetConvexRadius,
            "Convex radius of shape. Collision detection on penetrating shapes is much more expensive,\n"
            "so you can add a radius around objects to increase the shape. This makes it far less likely that they will actually penetrate.");

    nb::class_<ConvexShape::SupportBuffer> supportBufferCls(m, "SupportBuffer",
        "Buffer to hold a Support object, used to avoid dynamic memory allocations");
    // supportBufferCls
        // .def_rw("data", &ConvexShape::SupportBuffer::mData)// TODO:
        ;

    nb::enum_<ConvexShape::ESupportMode>(m, "ESupportMode",
        "How the GetSupport function should behave")
        .value("EXCLUDE_CONVEX_RADIUS", ConvexShape::ESupportMode::ExcludeConvexRadius,
            "Return the shape excluding the convex radius, Support::GetConvexRadius will return the convex radius if there is one, but adding this radius may not result in the most accurate/efficient representation of shapes with sharp edges")
        .value("INCLUDE_CONVEX_RADIUS", ConvexShape::ESupportMode::IncludeConvexRadius,
            "Return the shape including the convex radius, Support::GetSupport includes the convex radius if there is one, Support::GetConvexRadius will return 0")
        .value("DEFAULT", ConvexShape::ESupportMode::Default,
            "Use both Support::GetSupport add Support::GetConvexRadius to get a support point that matches the original shape as accurately/efficiently as possible");
}