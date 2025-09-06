#include "Common.h"
#include <Jolt/Core/MutexArray.h>

template <typename T>
static void BindInst(nb::module_ &m, std::string name) {
using CurrentInst = MutexArray<T>;
    nb::class_<CurrentInst>(m, (name + "MutexArray").c_str(), "A mutex array protects a number of resources with a limited amount of mutexes.\n"
                                     "It uses hashing to find the mutex of a particular object.\n"
                                     "The idea is that if the amount of threads is much smaller than the amount of mutexes\n"
                                     "that there is a relatively small chance that two different objects map to the same mutex.")

        .def(nb::init<>())
        .def(nb::init<uint>(), "num_mutexes"_a)

        .def("init", &CurrentInst::Init)
        .def("get_num_mutexes", &CurrentInst::GetNumMutexes, "Get the number of mutexes that were allocated")
        .def("get_mutex_index", &CurrentInst::GetMutexIndex, "object_index"_a, "Convert an object index to a mutex index")
        .def("get_mutex_by_object_index", &CurrentInst::GetMutexByObjectIndex, "in_object_index", "Get the mutex belonging to a certain object by index")
        .def("get_mutex_by_index", &CurrentInst::GetMutexByIndex, "mutex_index"_a, "Get a mutex by index in the array")
        // .def("lock_all", &CurrentInst::LockAll, "Lock all mutexes")
        // .def("unlock_all", &CurrentInst::UnlockAll, "Unlock all mutexes")
        ;
}

void BindMutexArray(nb::module_ &m) {
    BindInst<int>(m, "int");
}