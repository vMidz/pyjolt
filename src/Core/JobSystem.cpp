#include "Common.h"
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/trampoline.h>
#include <Jolt/Core/JobSystem.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Core/Profiler.h>
#include <Jolt/Core/NonCopyable.h>
#include <Jolt/Core/StaticArray.h>
#include <Jolt/Core/Atomics.h>
#include <Jolt/Core/JobSystemThreadPool.h>

class PyJobSystem : public JobSystem {
  public:
    NB_TRAMPOLINE(JobSystem, 8);

    int GetMaxConcurrency() const override {
        NB_OVERRIDE_PURE_NAME(
            "get_max_concurrency",
            GetMaxConcurrency);
    }

    JobHandle CreateJob(const char *inName, ColorArg inColor, const JobFunction &inJobFunction, uint32 inNumDependencies = 0) override {
        NB_OVERRIDE_PURE_NAME(
            "create_job",
            CreateJob,
            inName,
            inColor,
            inJobFunction,
            inNumDependencies);
    }

    Barrier *CreateBarrier() override {
        NB_OVERRIDE_PURE_NAME(
            "create_barrier",
            CreateBarrier);
    }

    void DestroyBarrier(Barrier *inBarrier) override {
        NB_OVERRIDE_PURE_NAME(
            "destroy_barrier",
            DestroyBarrier,
            inBarrier);
    }

    void WaitForJobs(Barrier *inBarrier) override {
        NB_OVERRIDE_PURE_NAME(
            "wait_for_jobs",
            WaitForJobs,
            inBarrier);
    }

  protected:
    void QueueJob(Job *inJob) override {
        NB_OVERRIDE_PURE_NAME(
            "queue_job",
            QueueJob,
            inJob);
    }

    void QueueJobs(Job **inJobs, uint inNumJobs) override {
        NB_OVERRIDE_PURE_NAME(
            "queue_jobs",
            QueueJobs,
            inJobs,
            inNumJobs);
    }

    void FreeJob(Job *inJob) override {
        NB_OVERRIDE_PURE_NAME(
            "free_job",
            FreeJob,
            inJob);
    }
};

class PyBarrier : public JobSystem::Barrier {
  public:
    NB_TRAMPOLINE(JobSystem::Barrier, 2);

    void AddJob(const JobHandle &inJob) override {
        NB_OVERRIDE_PURE_NAME(
            "add_job",
            AddJob,
            inJob);
    }

    void AddJobs(const JobHandle *inHandles, uint inNumHandles) override {
        NB_OVERRIDE_PURE_NAME(
            "add_jobs",
            AddJobs,
            inHandles,
            inNumHandles);
    }
};

class PythonCallableWrapper {
  public:
    PythonCallableWrapper(nb::callable callable) : mCallable(std::move(callable)) {
    }

    void operator()() {
        nb::gil_scoped_acquire acquire;
        try {
            mCallable();
        } catch (const std::exception &e) {
            PyErr_Print();
        }
    }

  private:
    nb::object mCallable;
};

const char *doc_string =
    "A class that allows units of work (Jobs) to be scheduled across multiple threads.\n"
    "It allows dependencies between the jobs so that the jobs form a graph.\n"
    "The pattern for using this class is:\n"
    "	// Create job system\n"
    "	JobSystem *job_system = new JobSystemThreadPool(...);\n"
    "	// Create some jobs\n"
    "	JobHandle second_job = job_system->CreateJob(\" SecondJob \", Color::sRed, []() { ... }, 1); // Create a job with 1 dependency\n"
    "	JobHandle first_job = job_system->CreateJob(\" FirstJob \", Color::sGreen, [second_job]() { ....; second_job.RemoveDependency(); }, 0); // Job can start immediately, will start second job when it's done\n"
    "	JobHandle third_job = job_system->CreateJob(\" ThirdJob \", Color::sBlue, []() { ... }, 0); // This job can run immediately as well and can run in parallel to job 1 and 2\n"
    "	// Add the jobs to the barrier so that we can execute them while we're waiting\n"
    "	Barrier *barrier = job_system->CreateBarrier();\n"
    "	barrier->AddJob(first_job);\n"
    "	barrier->AddJob(second_job);\n"
    "	barrier->AddJob(third_job);\n"
    "	job_system->WaitForJobs(barrier);\n"
    "	// Clean up\n"
    "	job_system->DestroyBarrier(barrier);\n"
    "	delete job_system;\n"
    "Jobs are guaranteed to be started in the order that their dependency counter becomes zero (in case they're scheduled on a background thread)\n"
    "or in the order they're added to the barrier (when dependency count is zero and when executing on the thread that calls WaitForJobs).\n"
    "If you want to implement your own job system, inherit from JobSystem and implement:\n"
    "* JobSystem::GetMaxConcurrency - This should return the maximum number of jobs that can run in parallel.\n"
    "* JobSystem::CreateJob - This should create a Job object and return it to the caller.\n"
    "* JobSystem::FreeJob - This should free the memory associated with the job object. It is called by the Job destructor when it is Release()-ed for the last time.\n"
    "* JobSystem::QueueJob/QueueJobs - These should store the job pointer in an internal queue to run immediately (dependencies are tracked internally, this function is called when the job can run).\n"
    "The Job objects are reference counted and are guaranteed to stay alive during the QueueJob(s) call. If you store the job in your own data structure you need to call AddRef() to take a reference.\n"
    "After the job has been executed you need to call Release() to release the reference. Make sure you no longer dereference the job pointer after calling Release().\n"
    "JobSystem::Barrier is used to track the completion of a set of jobs. Jobs will be created by other jobs and added to the barrier while it is being waited on. This means that you cannot\n"
    "create a dependency graph beforehand as the graph changes while jobs are running. Implement the following functions:\n"
    "* Barrier::AddJob/AddJobs - Add a job to the barrier, any call to WaitForJobs will now also wait for this job to complete.\n"
    "If you store the job in a data structure in the Barrier you need to call AddRef() on the job to keep it alive and Release() after you're done with it.\n"
    "* Barrier::OnJobFinished - This function is called when a job has finished executing, you can use this to track completion and remove the job from the list of jobs to wait on.\n"
    "The functions on JobSystem that need to be implemented to support barriers are:\n"
    "* JobSystem::CreateBarrier - Create a new barrier.\n"
    "* JobSystem::DestroyBarrier - Destroy a barrier.\n"
    "* JobSystem::WaitForJobs - This is the main function that is used to wait for all jobs that have been added to a Barrier. WaitForJobs can execute jobs that have\n"
    "been added to the barrier while waiting. It is not wise to execute other jobs that touch physics structures as this can cause race conditions and deadlocks. Please keep in mind that the barrier is\n"
    "only intended to wait on the completion of the Jolt jobs added to it, if you scheduled any jobs in your engine's job system to execute the Jolt jobs as part of QueueJob/QueueJobs, you might still need\n"
    "to wait for these in this function after the barrier is finished waiting.\n"
    "An example implementation is JobSystemThreadPool. If you don't want to write the Barrier class you can also inherit from JobSystemWithBarrier.";

void BindJobSystem(nb::module_ &m) {
    nb::class_<JobSystem, PyJobSystem> jobSystemCls(m, "JobSystem", doc_string);
    jobSystemCls
        .def(nb::init<>())
        .def("get_max_concurrency", &JobSystem::GetMaxConcurrency, "Get maximum number of concurrently executing jobs")
        .def("create_job", [](JobSystem &self, const char *name, const Color &color, nb::callable func, uint32_t num_dependencies) {
            JobSystem::JobFunction job_func = PythonCallableWrapper(std::move(func));
            return self.CreateJob(name, color, job_func, num_dependencies);
        }, "name"_a,
           "color"_a,
           "function"_a,
           "num_dependencies"_a = 0,
           "Create a new job, the job is started immediately if inNumDependencies == 0 otherwise it starts when\n"
           "RemoveDependency causes the dependency counter to reach 0.")

        .def("create_barrier", &JobSystem::CreateBarrier, "Create a new barrier, used to wait on jobs")
        .def("destroy_barrier", &JobSystem::DestroyBarrier,
            "Destroy a barrier when it is no longer used. The barrier should be empty at this point.")
        .def("wait_for_jobs", &JobSystem::WaitForJobs,
            "Wait for a set of jobs to be finished, note that only 1 thread can be waiting on a barrier at a time");

    // TODO: 'Job' is a protected member of 'JPH::JobSystem'
    nb::class_<JobSystem::JobHandle> jobHandleCls(jobSystemCls, "JobHandle",
        "A job handle contains a reference to a job. The job will be deleted as soon as there are no JobHandles.\n"
        "referring to the job and when it is not in the job queue / being processed.");
    jobHandleCls
        .def(nb::init<>())
        .def("is_valid", &JobSystem::JobHandle::IsValid, "Check if this handle contains a job")
        .def("is_done", &JobSystem::JobHandle::IsDone, "Check if this job has finished executing")
        .def("add_dependency", &JobSystem::JobHandle::AddDependency, "count"_a = 1, "Add to the dependency counter.")
        .def("remove_dependency", &JobSystem::JobHandle::RemoveDependency, "count"_a = 1,
            "Remove from the dependency counter. Job will start whenever the dependency counter reaches zero\n"
            "and if it does it is no longer valid to call the AddDependency/RemoveDependency functions.")
        .def_static("remove_dependencies", [](nb::list handles, int count) {
            std::vector<JobSystem::JobHandle> job_handles;
            for (auto handle : handles) {
                job_handles.push_back(nb::cast<JobSystem::JobHandle>(handle));
            }
            JobSystem::JobHandle::sRemoveDependencies(job_handles.data(), job_handles.size(), count);
        }, "handles"_a, "count"_a = 1,
           "Remove a dependency from a batch of jobs at once, this can be more efficient than removing them one by one as it requires less locking");

    nb::class_<JobSystem::Barrier, PyBarrier> barrierCls(jobSystemCls, "Barrier",
        "A job barrier keeps track of a number of jobs and allows waiting until they are all completed.");
    barrierCls
        .def("add_job", &JobSystem::Barrier::AddJob, " Add a job to this barrier\n"
                                                     "Note that jobs can keep being added to the barrier while waiting for the barrier")
        .def("add_jobs", [](JobSystem::Barrier &self, nb::list handles) {
            std::vector<JobSystem::JobHandle> job_handles;
            for (auto handle : handles) {
                job_handles.push_back(nb::cast<JobSystem::JobHandle>(handle));
            }
            self.AddJobs(job_handles.data(), job_handles.size());
        }, "Add multiple jobs to this barrier\n"
           "Note that jobs can keep being added to the barrier while waiting for the barrier");
}
