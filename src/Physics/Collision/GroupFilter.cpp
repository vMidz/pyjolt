#include "Common.h"
#include <Jolt/Physics/Collision/GroupFilter.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/CollisionGroup.h>
#include <Jolt/Core/StreamWrapper.h>
#include <Jolt/Core/Result.h>

#include <nanobind/trampoline.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/string.h>

class PyGroupFilter : public GroupFilter {
  public:
    NB_TRAMPOLINE(GroupFilter, 2);

    bool CanCollide(const CollisionGroup &inGroup1, const CollisionGroup &inGroup2) const override {
        NB_OVERRIDE_PURE_NAME(
            "can_collide",
            CanCollide,
            inGroup1,
            inGroup2);
    }

    void SaveBinaryState(StreamOut &inStream) const override {
        NB_OVERRIDE_NAME(
            "save_binary_state",
            SaveBinaryState,
            inStream);
    }
};

void BindGroupFilter(nb::module_ &m) {
    // TODO: multi inheritance
    nb::class_<GroupFilter, RefTarget<GroupFilter>>(m, "GroupFilter",
        nb::intrusive_ptr<GroupFilter>([](GroupFilter *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Abstract class that checks if two CollisionGroups collide")

        .def("can_collide", &GroupFilter::CanCollide, "group1"_a, "group2"_a, "Check if two groups collide")
        .def("save_binary_state", &GroupFilter::SaveBinaryState, "stream_"_a, "Saves the contents of the group filter in binary form to inStream.")
        .def_static("s_restore_from_binary_state", [](StreamIn &inStream) {
            return (GroupFilter::sRestoreFromBinaryState(inStream));
        }, "stream_in"_a, nb::rv_policy::take_ownership,
           "Creates a GroupFilter of the correct type and restores its contents from the binary stream inStream.");
}