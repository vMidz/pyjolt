#include "Common.h"
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Core/Mutex.h>

void BindBodyLockInterface(nb::module_ &m) {
    nb::class_<BodyLockInterface, NonCopyable> bodyLockInterfaceCls(m, "BodyLockInterface",
        "Base class interface for locking a body. Usually you will use BodyLockRead / BodyLockWrite / BodyLockMultiRead / BodyLockMultiWrite instead.");
    bodyLockInterfaceCls
        // .def(nb::init<BodyManager &>(), "body_manager"_a,
        //     "Constructor")
        .def("lock_read", nb::overload_cast<const BodyID &>(&BodyLockInterface::LockRead, nb::const_), "body_id"_a, nb::rv_policy::reference,
            "@name Locking functions\n")
        .def("unlock_read", nb::overload_cast<SharedMutex *>(&BodyLockInterface::UnlockRead, nb::const_), "mutex"_a)
        .def("lock_write", nb::overload_cast<const BodyID &>(&BodyLockInterface::LockWrite, nb::const_), "body_id"_a, nb::rv_policy::reference)
        .def("unlock_write", nb::overload_cast<SharedMutex *>(&BodyLockInterface::UnlockWrite, nb::const_), "mutex"_a)
        .def("get_all_bodies_mutex_mask", &BodyLockInterface::GetAllBodiesMutexMask,
            "Get the mask needed to lock all bodies")
        .def("get_mutex_mask", &BodyLockInterface::GetMutexMask, "bodies"_a, "number"_a,
            "@name Batch locking functions\n")
        .def("lock_read", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterface::LockRead, nb::const_), "mutex_mask"_a)
        .def("unlock_read", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterface::UnlockRead, nb::const_), "mutex_mask"_a)
        .def("lock_write", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterface::LockWrite, nb::const_), "mutex_mask"_a)
        .def("unlock_write", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterface::UnlockWrite, nb::const_), "mutex_mask"_a)
        .def("try_get_body", &BodyLockInterface::TryGetBody, "body_id"_a, nb::rv_policy::reference,
            "Convert body ID to body");

    nb::class_<BodyLockInterfaceNoLock, BodyLockInterface> bodyLockInterfaceNoLockCls(m, "BodyLockInterfaceNoLock",
        "Implementation that performs no locking (assumes the lock has already been taken)",
            nb::is_final());
    bodyLockInterfaceNoLockCls
        .def("lock_read", nb::overload_cast<const BodyID &>(&BodyLockInterfaceNoLock::LockRead, nb::const_), "body_id"_a, nb::rv_policy::reference,
            "@name Locking functions")
        .def("unlock_read", nb::overload_cast<SharedMutex *>(&BodyLockInterfaceNoLock::UnlockRead, nb::const_), "mutex"_a)
        .def("lock_write", nb::overload_cast<const BodyID &>(&BodyLockInterfaceNoLock::LockWrite, nb::const_), "body_id"_a, nb::rv_policy::reference)
        .def("unlock_write", nb::overload_cast<SharedMutex *>(&BodyLockInterfaceNoLock::UnlockWrite, nb::const_), "mutex"_a)
        .def("get_mutex_mask", &BodyLockInterfaceNoLock::GetMutexMask, "bodies"_a, "number"_a,
            "@name Batch locking functions")
        .def("lock_read", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceNoLock::LockRead, nb::const_), "mutex_mask"_a)
        .def("unlock_read", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceNoLock::UnlockRead, nb::const_), "mutex_mask"_a)
        .def("lock_write", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceNoLock::LockWrite, nb::const_), "mutex_mask"_a)
        .def("unlock_write", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceNoLock::UnlockWrite, nb::const_), "mutex_mask"_a);

    nb::class_<BodyLockInterfaceLocking, BodyLockInterface> bodyLockInterfaceLockingCls(m, "BodyLockInterfaceLocking",
        "Implementation that uses the body manager to lock the correct mutex for a body",
        nb::is_final());
    bodyLockInterfaceLockingCls
        .def("lock_read", nb::overload_cast<const BodyID &>(&BodyLockInterfaceLocking::LockRead, nb::const_), "body_id"_a, nb::rv_policy::reference,
            "@name Locking functions")
        .def("unlock_read", nb::overload_cast<SharedMutex *>(&BodyLockInterfaceLocking::UnlockRead, nb::const_), "mutex"_a)
        .def("lock_write", nb::overload_cast<const BodyID &>(&BodyLockInterfaceLocking::LockWrite, nb::const_), "body_id"_a, nb::rv_policy::reference)
        .def("unlock_write", nb::overload_cast<SharedMutex *>(&BodyLockInterfaceLocking::UnlockWrite, nb::const_), "mutex"_a)
        .def("get_mutex_mask", &BodyLockInterfaceLocking::GetMutexMask, "bodies"_a, "number"_a,
            "@name Batch locking functions")
        .def("lock_read", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceLocking::LockRead, nb::const_), "mutex_mask"_a)
        .def("unlock_read", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceLocking::UnlockRead, nb::const_), "mutex_mask"_a)
        .def("lock_write", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceLocking::LockWrite, nb::const_), "mutex_mask"_a)
        .def("unlock_write", nb::overload_cast<BodyManager::MutexMask>(&BodyLockInterfaceLocking::UnlockWrite, nb::const_), "mutex_mask"_a);
}