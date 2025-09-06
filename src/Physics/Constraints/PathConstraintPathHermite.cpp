#include "Common.h"
#include <Jolt/Physics/Constraints/PathConstraintPathHermite.h>
#include <Jolt/Core/StreamOut.h>

void BindPathConstraintPathHermite(nb::module_ &m) {
    nb::class_<PathConstraintPathHermite, PathConstraintPath> pathConstraintPathHermiteCls(m, "PathConstraintPathHermite",
        "A path that follows a Hermite spline",
        nb::is_final());
    pathConstraintPathHermiteCls
        .def("get_path_max_fraction", &PathConstraintPathHermite::GetPathMaxFraction)
        .def("get_closest_point", &PathConstraintPathHermite::GetClosestPoint, "position"_a, "fraction_hint"_a)
        .def("get_point_on_path",[](const PathConstraintPathHermite &self, float inFraction) {
            Vec3 outPathPosition;
            Vec3 outPathTangent;
            Vec3 outPathNormal;
            Vec3 outPathBinormal;
            self.GetPointOnPath(inFraction, outPathPosition, outPathTangent, outPathNormal, outPathBinormal);
            return nb::make_tuple(outPathPosition, outPathTangent, outPathNormal, outPathBinormal);
        }, "fraction"_a,
            "Output: [outPathPosition, outPathTangent, outPathNormal, outPathBinormal]")
        .def("add_point", &PathConstraintPathHermite::AddPoint, "position"_a, "tangent"_a, "normal"_a,
            "Adds a point to the path")
        .def("save_binary_state", &PathConstraintPathHermite::SaveBinaryState, "stream"_a);

    nb::class_<PathConstraintPathHermite::Point> pointCls(pathConstraintPathHermiteCls, "Point");
    pointCls
        .def_rw("position", &PathConstraintPathHermite::Point::mPosition,
            "Position on the path")
        .def_rw("tangent", &PathConstraintPathHermite::Point::mTangent,
            "Tangent of the path, does not need to be normalized (in the direction of the path)")
        .def_rw("normal", &PathConstraintPathHermite::Point::mNormal,
            "Normal of the path (together with the tangent along the curve this forms a basis for the constraint)");
}