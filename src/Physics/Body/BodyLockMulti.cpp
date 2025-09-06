#include "Common.h"
#include <Jolt/Physics/Body/BodyLockMulti.h>

template <bool Write, class BodyType>
static auto BindInst(nb::module_ &m, const char *name) {
    using CurrentInst = BodyLockMultiBase<Write, BodyType>;

    nb::class_<CurrentInst, NonCopyable> bodyLockMultiBaseCls(m, name,
        "Base class for locking multiple bodies for the duration of the scope of this class (do not use directly)");
    bodyLockMultiBaseCls
        .def(nb::init<const BodyLockInterface &, const BodyID *, int>(),
             "body_lock_interface"_a,
             "body_i_ds"_a,
             "number"_a,
             "Constructor will lock the bodies")
        .def("get_body", &CurrentInst::GetBody, "body_index"_a, nb::rv_policy::reference,
            "Access the body (returns null if body was not properly locked)");

    return bodyLockMultiBaseCls;
}

void BindBodyLockMulti(nb::module_ &m) {
    auto BodyLockMultiWriteInst = BindInst<true, Body>(m, "BodyLockMultiBaseWrite");
    auto BodyLockMultiReadInst = BindInst<false, const Body>(m, "BodyLockMultiBaseWrite");

    nb::class_<BodyLockMultiRead, BodyLockMultiBase<false, const Body>> bodyLockMultiReadCls(m, "BodyLockMultiRead",
        "A multi body lock takes a number of body IDs and locks the underlying bodies so that other threads cannot access its members\n"
        "The common usage pattern is:\n"
        "    BodyLockInterface lock_interface = physics_system.GetBodyLockInterface(); // Or non-locking interface if the lock is already taken\n"
        "    const BodyID *body_id = ...; // Obtain IDs to bodies\n"
        "    int num_body_ids = ...;\n"
        "    // Scoped lock\n"
        "    {\n"
        "        BodyLockMultiRead lock(lock_interface, body_ids, num_body_ids);\n"
        "        for (int i = 0; i < num_body_ids; ++i)\n"
        "        {\n"
        "            const Body *body = lock.GetBody(i);\n"
        "            if (body != nullptr)\n"
        "            {\n"
        "                const Body &body = lock.Body();\n"
        "                // Do something with body\n"
        "                ...\n"
        "            }\n"
        "        }\n"
        "    }");

    nb::class_<BodyLockMultiWrite, BodyLockMultiBase<true, Body>>(m, "BodyLockMultiWrite",
        "Specialization that locks multiple bodies for writing to.\n"
        "References:\n"
        "    BodyLockMultiRead for usage patterns.");
}