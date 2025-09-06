#include "Common.h"
#include <Jolt/Skeleton/SkeletalAnimation.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/ObjectStream/SerializableObject.h>
#include <Jolt/Skeleton/SkeletonPose.h>
#include <nanobind/stl/string.h>

void BindSkeletalAnimation(nb::module_ &m) {
    nb::class_<SkeletalAnimation, RefTarget<SkeletalAnimation>> skeletalAnimationCls(m, "SkeletalAnimation",
        nb::intrusive_ptr<SkeletalAnimation>([](SkeletalAnimation *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Resource for a skinned animation");
    skeletalAnimationCls
        .def("get_duration", &SkeletalAnimation::GetDuration,
            "Get the length (in seconds) of this animation")
        .def("scale_joints", &SkeletalAnimation::ScaleJoints, "scale"_a,
            "Scale the size of all joints by inScale")
        .def("sample", &SkeletalAnimation::Sample, "time"_a, "pose"_a,
            "Get the (interpolated) joint transforms at time inTime")
        .def("get_animated_joints", nb::overload_cast<>(&SkeletalAnimation::GetAnimatedJoints),
            "Get joint samples")
        .def("get_animated_joints", nb::overload_cast<>(&SkeletalAnimation::GetAnimatedJoints));

    nb::class_<SkeletalAnimation::JointState> jointStateCls(skeletalAnimationCls, "JointState",
            "Contains the current state of a joint, a local space transformation relative to its parent joint");
    jointStateCls
        .def("from_matrix", &SkeletalAnimation::JointState::FromMatrix, "matrix"_a,
            "Convert from a local space matrix")
        .def("to_matrix", &SkeletalAnimation::JointState::ToMatrix,
            "Convert to matrix representation")
        .def_rw("rotation", &SkeletalAnimation::JointState::mRotation,
            "Local space rotation of the joint")
        .def_rw("translation", &SkeletalAnimation::JointState::mTranslation,
            "Local space translation of the joint");

    nb::class_<SkeletalAnimation::Keyframe, SkeletalAnimation::JointState> keyframeCls(skeletalAnimationCls, "Keyframe",
        "Contains the state of a single joint at a particular time");
    keyframeCls
        .def_rw("time", &SkeletalAnimation::Keyframe::mTime,
            "Time of keyframe in seconds");

    nb::class_<SkeletalAnimation::AnimatedJoint> animatedJointCls(skeletalAnimationCls, "AnimatedJoint",
        "Contains the animation for a single joint");
    animatedJointCls
        .def_rw("joint_name", &SkeletalAnimation::AnimatedJoint::mJointName,
            "Name of the joint")
        .def_rw("keyframes", &SkeletalAnimation::AnimatedJoint::mKeyframes,
            "List of keyframes over time");
}