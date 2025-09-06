#include "Common.h"
#include <Jolt/Physics/PhysicsLock.h>
#include <Jolt/Physics/Body/BodyManager.h>

void BindPhysicsLock(nb::module_ &m) {
    nb::enum_<EPhysicsLockTypes>(m, "EPhysicsLockTypes",
        "This is the list of locks used by the physics engine, they need to be locked in a particular order (from top of the list to bottom of the list) in order to prevent deadlocks")
        .value("BROAD_PHASE_QUERY", EPhysicsLockTypes::BroadPhaseQuery)
        .value("PER_BODY", EPhysicsLockTypes::PerBody)
        .value("BODIES_LIST", EPhysicsLockTypes::BodiesList)
        .value("BROAD_PHASE_UPDATE", EPhysicsLockTypes::BroadPhaseUpdate)
        .value("CONSTRAINTS_LIST", EPhysicsLockTypes::ConstraintsList)
        .value("ACTIVE_BODIES_LIST", EPhysicsLockTypes::ActiveBodiesList);

    nb::class_<PhysicsLock> physicsLockCls(m, "PhysicsLock",
        "Helpers to safely lock the different mutexes that are part of the physics system while preventing deadlock\n"
        "Class that keeps track per thread which lock are taken and if the order of locking is correct");
    physicsLockCls
        .def_static("check_lock", &PhysicsLock::sCheckLock, "context"_a, "type"_a,
            "Call before taking the lock")
        .def_static("check_unlock", &PhysicsLock::sCheckUnlock, "context"_a, "type"_a,
            "Call after releasing the lock")
        .def_static("lock", &PhysicsLock::sLock<mutex>)
        .def_static("unlock", &PhysicsLock::sUnlock<mutex>)
        .def_static("lock_shared", &PhysicsLock::sLockShared<shared_mutex>)
        .def_static("unlock_shared", &PhysicsLock::sUnlockShared<shared_mutex>)
        ;

    nb::class_<UniqueLock<mutex>, NonCopyable> uniqueLockCls(m, "UniqueLock",
        "Helper class that is similar to std::unique_lock");
    uniqueLockCls
        .def(nb::init<mutex &, PhysicsLockContext, EPhysicsLockTypes>(), "lock"_a, "context"_a, "type"_a);

    nb::class_<SharedLock<SharedMutex>, NonCopyable> sharedLockCls(m, "SharedLock",
        "Helper class that is similar to std::shared_lock");
    sharedLockCls
        .def(nb::init<SharedMutex &, PhysicsLockContext, EPhysicsLockTypes>(), "lock"_a, "context"_a, "type"_a);
}