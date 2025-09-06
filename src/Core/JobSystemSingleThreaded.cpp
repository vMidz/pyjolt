#include "Common.h"
#include <Jolt/Core/JobSystemSingleThreaded.h>

#include <nanobind/stl/function.h>

void BindJobSystemSingleThreaded(nb::module_ &m) {
    nb::class_<JobSystemSingleThreaded, JobSystem>(m, "JobSystemSingleThreaded",
        "Implementation of a JobSystem without threads, runs jobs as soon as they are added")
        .def(nb::init<>(), "Constructor")
        .def(nb::init<uint>(), "max_jobs"_a)

        .def("init", &JobSystemSingleThreaded::Init, "max_jobs"_a,
            "Initialize the job system.\n"
            "Args:\n"
            "    max_jobs (int): Max number of jobs that can be allocated at any time")

        .def("get_max_concurrency", &JobSystemSingleThreaded::GetMaxConcurrency)
        .def("create_job", &JobSystemSingleThreaded::CreateJob, "name"_a, "color"_a, "job_funciton"_a, "num_dependencies"_a = 0)
        .def("create_barrier", &JobSystemSingleThreaded::CreateBarrier)
        .def("destroy_barrier", &JobSystemSingleThreaded::DestroyBarrier, "barrier"_a)
        .def("wait_for_jobs", &JobSystemSingleThreaded::WaitForJobs, "barrier"_a);
}