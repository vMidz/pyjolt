#include "Common.h"
#include <Jolt/Skeleton/Skeleton.h>
#include <Jolt/Core/StreamOut.h>
#include <Jolt/Core/StreamIn.h>
#include <nanobind/stl/string.h>

void BindSkeleton(nb::module_ &m) {
    nb::class_<Skeleton, RefTarget<Skeleton>> skeletonCls(m, "Skeleton",
        nb::intrusive_ptr<Skeleton>([](Skeleton *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Resource that contains the joint hierarchy for a skeleton");
    skeletonCls
        .def("get_joints", nb::overload_cast<>(&Skeleton::GetJoints),
            "@name Access to the joints")

        .def("get_joint_count", &Skeleton::GetJointCount)
        .def("get_joint", nb::overload_cast<int>(&Skeleton::GetJoint), "joint"_a)
        .def("add_joint", [](Skeleton &self, const char *name, const char *parentName){
            return self.AddJoint(name, parentName);
        })
        .def("add_joint", [](Skeleton &self, const char *name, int parentIndex){
            return self.AddJoint(name, parentIndex);
        })
        .def("get_joint_index", [](Skeleton &self, const char *name){
            return self.GetJointIndex(name);
        }, "name"_a,
           "Find joint by name")
        .def("calculate_parent_joint_indices", &Skeleton::CalculateParentJointIndices,
            "Fill in parent joint indices based on name")
        .def("are_joints_correctly_ordered", &Skeleton::AreJointsCorrectlyOrdered,
            "Many of the algorithms that use the Skeleton class require that parent joints are in the mJoints array before their children.\n"
            "This function returns true if this is the case, false if not.")
        .def("save_binary_state", &Skeleton::SaveBinaryState, "stream"_a,
            "Saves the state of this object in binary form to inStream.")
        .def_static("restore_from_binary_state", &Skeleton::sRestoreFromBinaryState, "stream"_a,
            "Restore the state of this object from inStream.");

    nb::class_<Skeleton::Joint> jointCls(skeletonCls, "Joint",
        "Declare internal structure for a joint");
    jointCls
        .def(nb::init<>())
        .def("__init__", [](Skeleton::Joint &self, const char *name, const char *parentName, int parentIndex){
            new(&self) Skeleton::Joint(name, parentName, parentIndex);
        })
        .def_rw("name", &Skeleton::Joint::mName,
            "Name of the joint")
        .def_rw("parent_name", &Skeleton::Joint::mParentName,
            "Name of parent joint")
        .def_rw("parent_joint_index", &Skeleton::Joint::mParentJointIndex,
            "Index of parent joint (in mJoints) or -1 if it has no parent");
}