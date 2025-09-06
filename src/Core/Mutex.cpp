#include "Common.h"
#include <mutex>
#include <Jolt/Core/Mutex.h>

void BindMutex(nb::module_ &m) {
    nb::class_<MutexBase>(m, "MutexBase", "On other platforms just use the STL implementation")
    .def(nb::init<>());

    nb::class_<SharedMutexBase>(m, "SharedMutexBase")
    .def(nb::init<>());

    nb::class_<Mutex, MutexBase>(m, "Mutex", "Very simple wrapper around MutexBase which tracks lock contention in the profiler\n"
                                             "and asserts that locks/unlocks take place on the same thread")
        .def(nb::init<>())
        .def("try_lock", &Mutex::try_lock)
        .def("lock", &Mutex::lock)
        .def("un_lock", &Mutex::unlock)
        .def("is_locked", &Mutex::is_locked);

    nb::class_<SharedMutex, SharedMutexBase>(m, "Mutex",
        "Very simple wrapper around SharedMutexBase which tracks lock contention in the profiler\n"
        "and asserts that locks/unlocks take place on the same thread")
        .def(nb::init<>())
        .def("try_lock", &SharedMutex::try_lock)
        .def("lock", &SharedMutex::lock)
        .def("un_lock", &SharedMutex::unlock)
        .def("is_locked", &SharedMutex::is_locked)
        .def("lock_shared", &SharedMutex::lock_shared);
}