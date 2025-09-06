#include "Common.h"
#include <Jolt/Core/JobSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>

void BindJobSystemThreadPool(nb::module_ &m) {
    nb::class_<JobSystemThreadPool, JobSystemWithBarrier>(m, "JobSystemThreadPool",
        "Implementation of a JobSystem using a thread pool\n"
        "Note that this is considered an example implementation. It is expected that when you integrate\n"
        "the physics engine into your own project that you'll provide your own implementation of the\n"
        "JobSystem built on top of whatever job system your project uses.")
        .def(nb::init<uint, uint, int>(), "max_jobs"_a, "max_barriers"_a, "num_threads"_a)
        .def(nb::init<>())

        .def("init", &JobSystemThreadPool::Init, "max_jobs"_a, "max_barriers"_a, "num_threads"_a = -1,
            "Initialize the thread pool.\n"
            "Args:\n"
            "    max_jobs (int): Max number of jobs that can be allocated at any time\n"
            "    max_barriers (int): Max number of barriers that can be allocated at any time\n"
            "    num_threads (int): Number of threads to start (the number of concurrent jobs is 1 more because the main thread will also run jobs while waiting for a barrier to complete). Use -1 to auto detect the amount of CPU's.")
        .def("get_max_concurrency", &JobSystemThreadPool::GetMaxConcurrency)
        .def("set_num_threads", &JobSystemThreadPool::SetNumThreads, "num_threads"_a, "Change the max concurrency after initialization")
        .def("set_thread_init_function", [](JobSystemThreadPool &self, nb::callable &func) {
            self.SetThreadInitFunction([&func](int threadIndex) {
                func(threadIndex);
            });
        }, "function"_a)
        .def("SetThreadExitFunction", [](JobSystemThreadPool &self, nb::callable &func) {
            self.SetThreadExitFunction([&func](int threadIndex) {
                func(threadIndex);
            });
        }, "function"_a);
}
