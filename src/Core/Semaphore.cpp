#include "Common.h"
#include <Jolt/Core/Semaphore.h>

void BindSemaphore(nb::module_ &m) {
    nb::class_<Semaphore>(m, "Semaphore", "Implements a semaphore")
        .def("release", &Semaphore::Release, "number"_a = 1, "Release the semaphore, signaling the thread waiting on the barrier that there may be work")
        .def("acquire", &Semaphore::Acquire, "number"_a = 1, "Acquire the semaphore inNumber times")
        .def("get_value", &Semaphore::GetValue, "Get the current value of the semaphore");
}