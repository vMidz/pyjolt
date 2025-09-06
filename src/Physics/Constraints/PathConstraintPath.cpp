#include "Common.h"
#include <Jolt/Physics/Constraints/PathConstraintPath.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Core/StreamIn.h>
#include <Jolt/Core/StreamOut.h>

void BindPathConstraintPath(nb::module_ &m) {
    nb::class_<PathConstraintPath, RefTarget<PathConstraintPath>> pathConstraintPathCls(m, "PathConstraintPath",
        nb::intrusive_ptr<PathConstraintPath>([](PathConstraintPath *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "The path for a path constraint. It allows attaching two bodies to each other while giving the second body the freedom to move along a path relative to the first.");
    pathConstraintPathCls
        .def("get_path_max_fraction", &PathConstraintPath::GetPathMaxFraction,
            "Gets the max fraction along the path. I.e. sort of the length of the path.")
        .def("get_closest_point", &PathConstraintPath::GetClosestPoint, "position"_a, "fraction_hint"_a,
            "Get the globally closest point on the curve (Could be slow!).\n"
            "Args:\n"
            "    position (Vec3): Position to find closest point for.\n"
            "    fraction_hint (float): Last known fraction along the path (can be used to speed up the search).\n"
            "Returns:\n"
            "    float: Fraction of closest point along the path.")
        .def("get_point_on_path", &PathConstraintPath::GetPointOnPath, "fraction"_a, "path_position"_a, "path_tangent"_a, "path_normal"_a, "path_binormal"_a,
            "Given the fraction along the path, get the point, tangent and normal.\n"
            "Args:\n"
            "    fraction (float): Fraction along the path [0, GetPathMaxFraction()].\n"
            "    path_position (Vec3): Returns the closest position to inSearchPosition on the path.\n"
            "    path_tangent (Vec3): Returns the tangent to the path at outPathPosition (the vector that follows the direction of the path).\n"
            "    path_normal (Vec3): Return the normal to the path at outPathPosition (a vector that's perpendicular to outPathTangent).\n"
            "    path_binormal (Vec3): Returns the binormal to the path at outPathPosition (a vector so that normal cross tangent = binormal).")
        .def("set_is_looping", &PathConstraintPath::SetIsLooping, "is_looping"_a,
            "If the path is looping or not. If a path is looping, the first and last point are automatically connected to each other. They should not be the same points.")
        .def("is_looping", &PathConstraintPath::IsLooping)
        .def("draw_path", &PathConstraintPath::DrawPath, "renderer"_a, "base_transform"_a,
            "Draw the path relative to inBaseTransform. Used for debug purposes.")
        .def("save_binary_state", &PathConstraintPath::SaveBinaryState, "stream"_a,
            "Saves the contents of the path in binary form to inStream.")
        .def_static("restore_from_binary_state", &PathConstraintPath::sRestoreFromBinaryState, "stream"_a,
            "Creates a Shape of the correct type and restores its contents from the binary stream inStream.");
}