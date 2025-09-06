#include "Common.h"
#include <Jolt/Physics/Body/BodyLock.h>

template <bool Write, class BodyType>
static auto BindInst(nb::module_ &m, const char *name) {
    using CurrentInst = BodyLockBase<Write, BodyType>;

    nb::class_<CurrentInst, NonCopyable> bodyLockBaseCls(m, name,
        "Base class for locking bodies for the duration of the scope of this class (do not use directly)");
    bodyLockBaseCls
        .def("release_lock", &CurrentInst::ReleaseLock,
            "Explicitly release the lock (normally this is done in the destructor)")
        .def("succeeded", &CurrentInst::Succeeded,
            "Test if the lock was successful (if the body ID was valid)")
        .def("succeeded_and_is_in_broad_phase", &CurrentInst::SucceededAndIsInBroadPhase,
            "Test if the lock was successful (if the body ID was valid) and the body is still in the broad phase")
        .def("get_body", &CurrentInst::GetBody,
            "Access the body");

    return bodyLockBaseCls;
}

void BindBodyLock(nb::module_ &m) {
    auto lockRead = BindInst<false, const Body>(m, "BodyLockBaseRead");
    auto lockWrite= BindInst<true, Body>(m, "BodyLockBaseWrite");

    nb::class_<BodyLockRead, BodyLockBase<false, const Body>> bodyLockReadCls(m, "BodyLockRead",
        "A body lock takes a body ID and locks the underlying body so that other threads cannot access its members\n"
        "The common usage pattern is:\n"
        "    BodyLockInterface lock_interface = physics_system.GetBodyLockInterface(); // Or non-locking interface if the lock is already taken\n"
        "    BodyID body_id = ...; // Obtain ID to body\n"
        "    // Scoped lock\n"
        "    {\n"
        "        BodyLockRead lock(lock_interface, body_id);\n"
        "        if (lock.Succeeded()) // body_id may no longer be valid\n"
        "        {\n"
        "            const Body &body = lock.GetBody();\n"
        "            // Do something with body\n"
        "            ...\n"
        "        }\n"
        "    }");

    nb::class_<BodyLockWrite, BodyLockBase<true, Body>> (m, "BodyLockWrite",
        "Specialization that locks a body for writing to.\n"
        "References:\n"
        "    BodyLockRead for usage patterns.");
}