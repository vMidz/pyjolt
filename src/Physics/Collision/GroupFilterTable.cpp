#include "Common.h"
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/GroupFilter.h>
#include <Jolt/Physics/Collision/CollisionGroup.h>
#include <Jolt/Core/StreamWrapper.h>
#include <Jolt/Core/Result.h>

#include <nanobind/stl/vector.h>

void BindGroupFilterTable(nb::module_ &m) {
    nb::class_<GroupFilterTable, GroupFilter> groupFilterTableCls(m, "GroupFilterTable",
        "Implementation of GroupFilter that stores a bit table with one bit per sub shape ID pair to determine if they collide or not\n"
        "The collision rules:\n"
        "- If one of the objects is in the cInvalidGroup the objects will collide.\n"
        "- If the objects are in different groups they will collide.\n"
        "- If they're in the same group but their collision filter is different they will not collide.\n"
        "- If they're in the same group and their collision filters match, we'll use the SubGroupID and the table below.\n"
        "For N = 6 sub groups the table will look like:\n"
        "                sub group 1 --->\n"
        "    sub group 2 x.....\n"
        "         |      ox....\n"
        "         |      oox...\n"
        "         V      ooox..\n"
        "                oooox.\n"
        "                ooooox\n"
        "* 'x' means sub group 1 == sub group 2 and we define this to never collide.\n"
        "* 'o' is a bit that we have to store that defines if the sub groups collide or not.\n"
        "* '.' is a bit we don't need to store because the table is symmetric, we take care that group 2 > group 1 by swapping sub group 1 and sub group 2 if needed.\n"
        "The total number of bits we need to store is (N * (N - 1)) / 2",
        nb::is_final());
    groupFilterTableCls
        .def(nb::init<uint>(), "num_sub_groups"_a = 0,
            "Constructs the table with inNumSubGroups subgroups, initially all collision pairs are enabled except when the sub group ID is the same")
        .def(nb::init<const GroupFilterTable &>(), "rhs"_a,
            "Copy constructor")
        .def("disable_collision", &GroupFilterTable::DisableCollision, "sub_group1"_a, "sub_group2"_a,
            "Disable collision between two sub groups")
        .def("enable_collision", &GroupFilterTable::EnableCollision, "sub_group1"_a, "sub_group2"_a,
            "Enable collision between two sub groups")
        .def("is_collision_enabled", &GroupFilterTable::IsCollisionEnabled, "sub_group1"_a, "sub_group2"_a,
            "Check if the collision between two subgroups is enabled")
        .def("can_collide", &GroupFilterTable::CanCollide, "group1"_a, "group2"_a,
            "Checks if two CollisionGroups collide")
        .def("save_binary_state", &GroupFilterTable::SaveBinaryState, "stream"_a);
}