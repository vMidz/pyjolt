#include "Common.h"
#include <Jolt/Physics/Collision/ShapeCast.h>

template <class Vec, class Mat, class ShapeCastType>
static auto BindInst(nb::module_ &m, const char *name) {
    using CurrentInst = ShapeCastT<Vec, Mat, ShapeCastType>;

    nb::class_<CurrentInst> cls(m, name, "Structure holding data for casting a shape (single precision).");
    cls
        .def(nb::init<const Shape *, Vec3Arg, Mat, Vec3Arg, const AABox &>(), "shape"_a, "scale"_a, "center_of_mass_start"_a, "direction"_a, "world_space_bounds"_a,
            "Constructor")
        .def(nb::init<const Shape *, Vec3Arg, Mat, Vec3Arg>(), "shape"_a, "scale"_a, "center_of_mass_start"_a, "direction"_a,
            "Constructor")
        .def_static("from_world_transform", &CurrentInst::sFromWorldTransform, "shape"_a, "scale"_a, "world_transform"_a, "direction"_a,
            "Construct a shape cast using a world transform for a shape instead of a center of mass transform")
        .def("post_transformed", &CurrentInst::PostTransformed, "transform"_a,
            "Transform this shape cast using inTransform. Multiply transform on the left left hand side.")
        .def("post_translated", &CurrentInst::PostTranslated, "translation"_a,
            "Translate this shape cast by inTranslation.")
        .def("get_point_on_ray", &CurrentInst::GetPointOnRay, "fraction"_a,
            "Get point with fraction inFraction on ray from mCenterOfMassStart to mCenterOfMassStart + mDirection (0 = start of ray, 1 = end of ray)")
        .def_rw("shape", &CurrentInst::mShape,
            "Shape that's being cast (cannot be mesh shape). Note that this structure does not assume ownership over the shape for performance reasons.")
        .def_ro("SCALE", &CurrentInst::mScale,
            "Scale in local space of the shape being cast (scales relative to its center of mass)")
        .def_ro("CENTER_OF_MASS_START", &CurrentInst::mCenterOfMassStart,
            "Start position and orientation of the center of mass of the shape (construct using sFromWorldTransform if you have a world transform for your shape)")
        .def_ro("DIRECTION", &CurrentInst::mDirection,
            "Direction and length of the cast (anything beyond this length will not be reported as a hit)")
        .def_ro("SHAPE_WORLD_BOUNDS", &CurrentInst::mShapeWorldBounds,
            "Cached shape's world bounds, calculated in constructor");

    return cls;
}

void BindShapeCast(nb::module_ &m) {
    nb::class_<ShapeCastSettings, CollideSettingsBase> shapeCastSettingsCls(m, "ShapeCastSettings",
        "Settings to be passed with a shape cast");
    shapeCastSettingsCls
        .def(nb::init<>())
        .def("set_back_face_mode", &ShapeCastSettings::SetBackFaceMode, "mode"_a,
            "Set the backfacing mode for all shapes")
        .def_rw("back_face_mode_triangles", &ShapeCastSettings::mBackFaceModeTriangles,
            "How backfacing triangles should be treated (should we report moving from back to front for triangle based shapes, e.g. for MeshShape/HeightFieldShape?)")
        .def_rw("back_face_mode_convex", &ShapeCastSettings::mBackFaceModeConvex,
            "How backfacing convex objects should be treated (should we report starting inside an object and moving out?)")
        .def_rw("use_shrunken_shape_and_convex_radius", &ShapeCastSettings::mUseShrunkenShapeAndConvexRadius,
            "Indicates if we want to shrink the shape by the convex radius and then expand it again. This speeds up collision detection and gives a more accurate normal at the cost of a more 'rounded' shape.")
        .def_rw("return_deepest_point", &ShapeCastSettings::mReturnDeepestPoint,
            "When true, and the shape is intersecting at the beginning of the cast (fraction = 0) then this will calculate the deepest penetration point (costing additional CPU time)");

    nb::class_<ShapeCastResult, CollideShapeResult> shapeCastResultCls(m, "ShapeCastResult",
        "Result of a shape cast test");
    shapeCastResultCls
        .def(nb::init<>(),
            "Default constructor")
        .def(nb::init<float, Vec3Arg, Vec3Arg, Vec3Arg, bool, const SubShapeID &,
            const SubShapeID &, const BodyID &>(),
            "fraction"_a, "contact_point1"_a, "contact_point2"_a,
            "contact_normal_or_penetration_depth"_a, "back_face_hit"_a,
            "sub_shape_id1"_a, "sub_shape_id2"_a, "body_id2"_a,
            "Constructor.\n"
            "Args:\n"
            "    fraction (float): Fraction at which the cast hit.\n"
            "    contact_point1 (Vec3): Contact point on shape 1.\n"
            "    contact_point2 (Vec3): Contact point on shape 2.\n"
            "    contact_normal_or_penetration_depth (Vec3): Contact normal pointing from shape 1 to 2 or penetration depth vector when the objects are penetrating (also from 1 to 2).\n"
            "    back_face_hit (bool): If this hit was a back face hit.\n"
            "    sub_shape_id1 (SubShapeID): Sub shape id for shape 1.\n"
            "    sub_shape_id2 (SubShapeID): Sub shape id for shape 2.\n"
            "    body_id2 (BodyID): BodyID that was hit.")
        .def("get_early_out_fraction", &ShapeCastResult::GetEarlyOutFraction,
            "Function required by the CollisionCollector. A smaller fraction is considered to be a 'better hit'. For rays/cast shapes we can just use the collision fraction. The fraction and penetration depth are combined in such a way that deeper hits at fraction 0 go first.")
        .def("reversed", &ShapeCastResult::Reversed, "world_space_cast_direction"_a,
            "Reverses the hit result, swapping contact point 1 with contact point 2 etc.\n"
            "Args:\n"
            "    world_space_cast_direction (Vec3): Direction of the shape cast in world space.")
        .def_rw("fraction", &ShapeCastResult::mFraction,
            "This is the fraction where the shape hit the other shape: CenterOfMassOnHit = Start + value * (End - Start)")
        .def_rw("is_back_face_hit", &ShapeCastResult::mIsBackFaceHit,
            "True if the shape was hit from the back side");

    auto cast = BindInst<Vec3, Mat44, ShapeCast>(m, "ShapeCastT");
    auto castR = BindInst<RVec3, RMat44, RShapeCast>(m, "ShapeCastTR");

    nb::class_<ShapeCast, ShapeCastT<Vec3, Mat44, ShapeCast>> shapeCastCls(m, "ShapeCast");
    // shapeCastCls;

    nb::class_<RShapeCast, ShapeCastT<RVec3, RMat44, RShapeCast>> rShapeCastCls(m, "RShapeCast");
    rShapeCastCls
        .def(nb::init<const ShapeCast &>(), "cast"_a,
            "Convert from ShapeCast, converts single to double precision")
        .def("__init__", [](RShapeCast &self, const Shape *inShape, Vec3Arg inScale, Mat44 &inCenterOfMassStart, Vec3 &inDirection, const AABox &inWorldSpaceBounds){
            new (&self) RShapeCast(inShape, inScale, inCenterOfMassStart, inDirection, inWorldSpaceBounds);
        })
        .def("__init__", [](RShapeCast &self, const Shape *inShape, Vec3Arg inScale, Mat44 &inCenterOfMassStart, Vec3 &inDirection){
            new (&self) RShapeCast(inShape, inScale, inCenterOfMassStart, inDirection);
        })
        .def("to_shape_cast", &RShapeCast::operator ShapeCast,
            "Convert to ShapeCast, which implies casting from double precision to single precision");

}