#include "Common.h"
#include <Jolt/Physics/Body/BodyAccess.h>

void BindBodyAccess(nb::module_ &m) {
    nb::class_<BodyAccess> bodyAccessCls(m, "BodyAccess");

    nb::enum_<BodyAccess::EAccess>(bodyAccessCls, "EAccess",
        "Access rules, used to detect race conditions during simulation")
        .value("NONE", BodyAccess::EAccess::None)
        .value("READ", BodyAccess::EAccess::Read)
        .value("READ_WRITE", BodyAccess::EAccess::ReadWrite);

    nb::class_<BodyAccess::Grant>(bodyAccessCls, "Grant", "Grant a scope specific access rights on the current thread")
        .def(nb::init<BodyAccess::EAccess, BodyAccess::EAccess>(), "velocity"_a, "position"_a)
        .def("__enter__", [](BodyAccess::Grant &self) -> BodyAccess::Grant & {
            return self;
        })
        .def("__exit__", [](BodyAccess::Grant &self, nb::args args) -> bool {
            return false;
        });

    bodyAccessCls
        .def_static("check_rights", &BodyAccess::sCheckRights, "current_rights"_a, "desired_rights"_a, "Check if we have permission")
        .def_static("velocity_access", &BodyAccess::sVelocityAccess, "Access to read/write velocities")
        .def_static("position_access", &BodyAccess::sPositionAccess, "Access to read/write positions");
}