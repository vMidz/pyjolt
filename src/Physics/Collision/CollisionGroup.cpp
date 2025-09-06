#include "Common.h"
#include <Jolt/Physics/Collision/CollisionGroup.h>
#include <Jolt/Core/StreamIn.h>
#include <Jolt/Core/StreamOut.h>

void BindCollisionGroup(nb::module_ &m) {
    nb::class_<CollisionGroup> collisionGroupCls(m, "CollisionGroup",
        "Two objects collide with each other if:\n"
        "- Both don't have a group filter\n"
        "- The first group filter says that the objects can collide\n"
        "- Or if there's no filter for the first object, the second group filter says the objects can collide");
    collisionGroupCls
        .def(nb::init<>(),
            "Default constructor")
        .def(nb::init<const GroupFilter *, CollisionGroup::GroupID, CollisionGroup::SubGroupID>(), "filter"_a, "group_id"_a, "sub_group_id"_a,
            "Construct with all properties")
        .def("set_group_filter", &CollisionGroup::SetGroupFilter, "filter"_a,
            "Set the collision group filter")
        .def("get_group_filter", &CollisionGroup::GetGroupFilter, nb::rv_policy::reference,
            "Get the collision group filter")
        .def("set_group_id", &CollisionGroup::SetGroupID, "id"_a,
            "Set the main group id for this object")
        .def("get_group_id", &CollisionGroup::GetGroupID)
        .def("set_sub_group_id", &CollisionGroup::SetSubGroupID, "id"_a,
            "Add this object to a sub group")
        .def("get_sub_group_id", &CollisionGroup::GetSubGroupID)
        .def("can_collide", &CollisionGroup::CanCollide, "other"_a,
            "Check if this object collides with another object")
        .def("save_binary_state", &CollisionGroup::SaveBinaryState, "stream"_a,
            "Saves the state of this object in binary form to inStream. Does not save group filter.")
        .def("restore_binary_state", &CollisionGroup::RestoreBinaryState, "stream"_a,
            "Restore the state of this object from inStream. Does not save group filter.")
        .def_ro_static("C_INVALID_GROUP", &CollisionGroup::cInvalidGroup)
        .def_ro_static("C_INVALID_SUB_GROUP", &CollisionGroup::cInvalidSubGroup);
}