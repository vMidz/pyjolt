#include "Common.h"
#include <Jolt/Physics/Collision/RayCast.h>

template <class Vec, class Mat, class RayCastType>
static auto BindInst(nb::module_ &m, const char * name) {
    using CurrentInst =  RayCastT<Vec, Mat, RayCastType>;

    nb::class_<CurrentInst> rayCastTCls(m, name,
            "Structure that holds a single ray cast");
    rayCastTCls
        .def(nb::init<>(),
            "Constructors")
        .def(nb::init<Vec, Vec3Arg>(), "origin"_a, "direction"_a)
        .def(nb::init<const RayCastT<Vec, Mat, RayCastType> &>())
        .def("transformed", &CurrentInst::Transformed, "transform"_a,
            "Transform this ray using inTransform")
        .def("translated", &CurrentInst::Translated, "translation"_a,
            "Translate ray using inTranslation")
        .def("get_point_on_ray", &CurrentInst::GetPointOnRay, "fraction"_a,
            "Get point with fraction inFraction on ray (0 = start of ray, 1 = end of ray)")
        .def_rw("origin", &CurrentInst::mOrigin,
            "Origin of the ray")
        .def_rw("direction", &CurrentInst::mDirection,
            "Direction and length of the ray (anything beyond this length will not be reported as a hit)");

    return rayCastTCls;
}

void BindRayCast(nb::module_ &m) {
    nb::class_<RayCastSettings> rayCastSettingsCls(m, "RayCastSettings",
        "Settings to be passed with a ray cast");
    rayCastSettingsCls
        .def("set_back_face_mode", &RayCastSettings::SetBackFaceMode, "mode"_a,
            "Set the backfacing mode for all shapes")
        .def_rw("back_face_mode_triangles", &RayCastSettings::mBackFaceModeTriangles,
            "How backfacing triangles should be treated (should we report back facing hits for triangle based shapes, e.g. MeshShape/HeightFieldShape?)")
        .def_rw("back_face_mode_convex", &RayCastSettings::mBackFaceModeConvex,
            "How backfacing convex objects should be treated (should we report back facing hits for convex shapes?)")
        .def_rw("treat_convex_as_solid", &RayCastSettings::mTreatConvexAsSolid,
            "If convex shapes should be treated as solid. When true, a ray starting inside a convex shape will generate a hit at fraction 0.");

    auto castCls = BindInst<Vec3, Mat44, RayCast>(m, "RayCastTVec3Mat44RayCast");
    auto rCastCls = BindInst<RVec3, RMat44, RRayCast>(m, "RayCastTRVec3RMat44RRayCast");

    nb::class_<RayCast, RayCastT<Vec3, Mat44, RayCast>> rayCastCls(m, "RayCast");

    nb::class_<RRayCast, RayCastT<RVec3, RMat44, RRayCast>> rRayCastCls(m, "RRayCast");
    rRayCastCls
        .def(nb::init<const RayCast &>(), "ray"_a,
            "Convert from RayCast, converts single to double precision")
        .def("to_ray_cast", &RRayCast::operator RayCast,
            "Convert to RayCast, which implies casting from double precision to single precision");
}