#include "Common.h"
#include <Jolt/Skeleton/SkeletonMapper.h>

#include <nanobind/stl/function.h>

void BindSkeletonMapper(nb::module_ &m) {
    nb::class_<SkeletonMapper, RefTarget<SkeletonMapper>> skeletonMapperCls(m, "SkeletonMapper",
        nb::intrusive_ptr<SkeletonMapper>([](SkeletonMapper *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Class that is able to map a low detail (ragdoll) skeleton to a high detail (animation) skeleton and vice versa");
    skeletonMapperCls
        .def(nb::init<>())
        .def_static("default_can_map_joint", &SkeletonMapper::sDefaultCanMapJoint, "skeleton1"_a, "index1"_a, "skeleton2"_a, "index2"_a,
            "Default function that checks if the names of the joints are equal")
        .def("initialize", &SkeletonMapper::Initialize, "skeleton1"_a, "neutral_pose1"_a, "skeleton2"_a, "neutral_pose2"_a, "can_map_joint"_a, // TODO: can_map_joint"_a = SkeletonMapper::sDefaultCanMapJoint
            "Initialize the skeleton mapper. Skeleton 1 should be the (low detail) ragdoll skeleton and skeleton 2 the (high detail) animation skeleton.\n"
            "We assume that each joint in skeleton 1 can be mapped to a joint in skeleton 2 (if not mapping from animation skeleton to ragdoll skeleton will be undefined).\n"
            "Skeleton 2 should have the same hierarchy as skeleton 1 but can contain extra joints between those in skeleton 1 and it can have extra joints at the root and leaves of the skeleton.\n"
            "Args:\n"
            "    skeleton1 (Skeleton*): Source skeleton to map from.\n"
            "    neutral_pose1 (Mat44*): Neutral pose of the source skeleton (model space).\n"
            "    skeleton2 (Skeleton*): Target skeleton to map to.\n"
            "    neutral_pose2 (Mat44*): Neutral pose of the target skeleton (model space), inNeutralPose1 and inNeutralPose2 must match as closely as possible, preferably the position of the mappable joints should be identical.\n"
            "    can_map_joint (CanMapJoint): Function that checks if joints in skeleton 1 and skeleton 2 are equal.")
        .def("lock_translations", &SkeletonMapper::LockTranslations, "skeleton2"_a, "locked_translations"_a, "neutral_pose2"_a,
            "This can be called so lock the translation of a specified set of joints in skeleton 2.\n"
            "Because constraints are never 100% rigid, there's always a little bit of stretch in the ragdoll when the ragdoll is under stress.\n"
            "Locking the translations of the pose will remove the visual stretch from the ragdoll but will introduce a difference between the\n"
            "physical simulation and the visual representation.\n"
            "Args:\n"
            "    skeleton2 (Skeleton*): Target skeleton to map to.\n"
            "    locked_translations (bool*): An array of bools the size of inSkeleton2->GetJointCount(), for each joint indicating if the joint is locked.\n"
            "    neutral_pose2 (Mat44*): Neutral pose to take reference translations from.")
        .def("lock_all_translations", &SkeletonMapper::LockAllTranslations, "skeleton2"_a, "neutral_pose2"_a,
            "After Initialize(), this can be called to lock the translation of all joints in skeleton 2 below the first mapped joint to those of the neutral pose.\n"
            "Because constraints are never 100% rigid, there's always a little bit of stretch in the ragdoll when the ragdoll is under stress.\n"
            "Locking the translations of the pose will remove the visual stretch from the ragdoll but will introduce a difference between the\n"
            "physical simulation and the visual representation.\n"
            "Args:\n"
            "    skeleton2 (Skeleton*): Target skeleton to map to.\n"
            "    neutral_pose2 (Mat44*): Neutral pose to take reference translations from.")
        .def("map", &SkeletonMapper::Map, "pose1_model_space"_a, "pose2_local_space"_a, "pose2_model_space"_a,
            "Map a pose. Joints that were directly mappable will be copied in model space from pose 1 to pose 2. Any joints that are only present in skeleton 2\n"
            "will get their model space transform calculated through the local space transforms of pose 2. Joints that are part of a joint chain between two\n"
            "mapped joints will be reoriented towards the next joint in skeleton 1. This means that it is possible for unmapped joints to have some animation,\n"
            "but very extreme animation poses will show artifacts.\n"
            "Args:\n"
            "    pose1_model_space (Mat44*): Pose on skeleton 1 in model space.\n"
            "    pose2_local_space (Mat44*): Pose on skeleton 2 in local space (used for the joints that cannot be mapped).\n"
            "    pose2_model_space (Mat44*): Model space pose on skeleton 2 (the output of the mapping).")
        .def("map_reverse", &SkeletonMapper::MapReverse, "pose2_model_space"_a, "pose1_model_space"_a,
            "Reverse map a pose, this will only use the mappings and not the chains (it assumes that all joints in skeleton 1 are mapped).\n"
            "Args:\n"
            "    pose2_model_space (Mat44*): Model space pose on skeleton 2.\n"
            "    pose1_model_space (Mat44*): When the function returns this will contain the model space pose for skeleton 1.")
        .def("get_mapped_joint_idx", &SkeletonMapper::GetMappedJointIdx, "joint1_idx"_a,
            "Search through the directly mapped joints (mMappings) and find inJoint1Idx, returns the corresponding Joint2Idx or -1 if not found.")
        .def("is_joint_translation_locked", &SkeletonMapper::IsJointTranslationLocked, "joint2_idx"_a,
            "Search through the locked translations (mLockedTranslations) and find if joint inJoint2Idx is locked.")
        .def("get_mappings", nb::overload_cast<>(&SkeletonMapper::GetMappings),
            "@name Access to the mapped joints\n")
        .def("get_chains", nb::overload_cast<>(&SkeletonMapper::GetChains))
        .def("get_unmapped", nb::overload_cast<>(&SkeletonMapper::GetUnmapped))
        .def("get_locked_translations", nb::overload_cast<>(&SkeletonMapper::GetLockedTranslations))
        .def("get_locked_translations", nb::overload_cast<>(&SkeletonMapper::GetLockedTranslations));

    nb::class_<SkeletonMapper::Mapping> mappingCls(skeletonMapperCls, "Mapping",
        "A joint that maps 1-on-1 to a joint in the other skeleton");
    mappingCls
        .def(nb::init<>())
        .def(nb::init<int, int, Mat44Arg>(), "joint_idx1"_a, "joint_idx2"_a, "joint1_to2"_a)
        .def_rw("joint_idx1", &SkeletonMapper::Mapping::mJointIdx1,
            "Index of joint from skeleton 1")
        .def_rw("joint_idx2", &SkeletonMapper::Mapping::mJointIdx2,
            "Corresponding index of joint from skeleton 2")
        .def_rw("joint1_to2", &SkeletonMapper::Mapping::mJoint1To2,
            "Transforms this joint from skeleton 1 to 2")
        .def_rw("joint2_to1", &SkeletonMapper::Mapping::mJoint2To1,
            "Inverse of the transform above");

    nb::class_<SkeletonMapper::Chain> chainCls(skeletonMapperCls, "Chain",
        "A joint chain that starts with a 1-on-1 mapped joint and ends with a 1-on-1 mapped joint with intermediate joints that cannot be mapped");
    chainCls
        .def(nb::init<>())
        .def(nb::init<Array<int> &&, Array<int> &&>(), "joint_indices1"_a, "joint_indices2"_a)
        .def_rw("joint_indices1", &SkeletonMapper::Chain::mJointIndices1,
            "Joint chain from skeleton 1")
        .def_rw("joint_indices2", &SkeletonMapper::Chain::mJointIndices2,
            "Corresponding joint chain from skeleton 2");

    nb::class_<SkeletonMapper::Unmapped> unmappedCls(skeletonMapperCls, "Unmapped",
        "Joints that could not be mapped from skeleton 1 to 2");
    unmappedCls
        .def(nb::init<>())
        .def(nb::init<int, int>(), "joint_idx"_a, "parent_joint_idx"_a)
        .def_rw("joint_idx", &SkeletonMapper::Unmapped::mJointIdx,
            "Joint index of unmappable joint")
        .def_rw("parent_joint_idx", &SkeletonMapper::Unmapped::mParentJointIdx,
            "Parent joint index of unmappable joint");

    nb::class_<SkeletonMapper::Locked> lockedCls(skeletonMapperCls, "Locked",
        "Joints that should have their translation locked (fixed)");
    lockedCls
        .def(nb::init<>())
        .def_rw("joint_idx", &SkeletonMapper::Locked::mJointIdx,
            "Joint index of joint with locked translation (in skeleton 2)")
        .def_rw("parent_joint_idx", &SkeletonMapper::Locked::mParentJointIdx,
            "Parent joint index of joint with locked translation (in skeleton 2)")
        .def_rw("translation", &SkeletonMapper::Locked::mTranslation,
            "Translation of neutral pose");
}