#include "Common.h"
#include <Jolt/Skeleton/SkeletonPose.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindSkeletonPose(nb::module_ &m) {
    nb::class_<SkeletonPose> skeletonPoseCls(m, "SkeletonPose",
        "Instance of a skeleton, contains the pose the current skeleton is in");
    skeletonPoseCls
        .def("set_skeleton", &SkeletonPose::SetSkeleton, "skeleton"_a,
            "@name Skeleton\n")

        .def("get_skeleton", &SkeletonPose::GetSkeleton, nb::rv_policy::reference)
        .def("set_root_offset", &SkeletonPose::SetRootOffset, "offset"_a,
            "Extra offset applied to the root (and therefore also to all of its children)")
        .def("get_root_offset", &SkeletonPose::GetRootOffset)
        .def("get_joint_count", &SkeletonPose::GetJointCount,
            "@name Properties of the joints")

        .def("get_joints", nb::overload_cast<>(&SkeletonPose::GetJoints))
        .def("get_joint", nb::overload_cast<int>(&SkeletonPose::GetJoint), "joint"_a)
        .def("get_joint_matrices", nb::overload_cast<>(&SkeletonPose::GetJointMatrices),
            "@name Joint matrices")
        .def("get_joint_matrix", nb::overload_cast<int>(&SkeletonPose::GetJointMatrix), "joint"_a)
        .def("calculate_joint_matrices", &SkeletonPose::CalculateJointMatrices,
            "Convert the joint states to joint matrices")
        .def("calculate_joint_states", &SkeletonPose::CalculateJointStates,
            "Convert joint matrices to joint states")
        .def("calculate_local_space_joint_matrices", &SkeletonPose::CalculateLocalSpaceJointMatrices, "matrices"_a,
            "Outputs the joint matrices in local space (ensure that outMatrices has GetJointCount() elements, assumes that values in GetJoints() is up to date)")
        .def("draw", &SkeletonPose::Draw, "draw_settings"_a, "renderer"_a, "offset"_a = RMat44::sIdentity (),
            "Draw current pose");

    nb::class_<SkeletonPose::DrawSettings> drawSettingsCls(skeletonPoseCls, "DrawSettings",
        "Draw settings");
    drawSettingsCls
        .def(nb::init<>())
        .def_rw("draw_joints", &SkeletonPose::DrawSettings::mDrawJoints)
        .def_rw("draw_joint_orientations", &SkeletonPose::DrawSettings::mDrawJointOrientations)
        .def_rw("draw_joint_names", &SkeletonPose::DrawSettings::mDrawJointNames);
}
