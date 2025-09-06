#include "Common.h"
#include <Jolt/Core/JobSystemWithBarrier.h>

void BindJobSystemWithBarrier(nb::module_ &m) {
    nb::class_<JobSystemWithBarrier, JobSystem>(m, "JobSystemWithBarrier",
        "Implementation of the Barrier class for a JobSystem\n"
        " This class can be used to make it easier to create a new JobSystem implementation that integrates with your own job system.\n"
        " It will implement all functionality relating to barriers, so the only functions that are left to be implemented are:\n"
        " * JobSystem::GetMaxConcurrency\n"
        " * JobSystem::CreateJob\n"
        " * JobSystem::FreeJob\n"
        " * JobSystem::QueueJob/QueueJobs\n"
        " See instructions in JobSystem for more information on how to implement these.")
        // .def(nb::init<>(), "max_barriers"_a, "Constructs barriers")

        .def("init", &JobSystemWithBarrier::Init, "max_barriers"_a, "Initialize the barriers\n"
            "Initialize the barriers.\n"
            "Args:\n"
            "    max_barriers (int): Max number of barriers that can be allocated at any time")

        .def("create_barrier", &JobSystemWithBarrier::CreateBarrier)
        .def("destroy_barrier", &JobSystemWithBarrier::DestroyBarrier, "barrier"_a)
        .def("wait_for_jobs", &JobSystemWithBarrier::WaitForJobs, "barrier"_a)
        ;
}