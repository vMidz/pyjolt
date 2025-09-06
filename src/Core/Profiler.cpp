#include "Common.h"
#include <Jolt/Core/Profiler.h>
#include <nanobind/stl/string.h>
#include <nanobind/ndarray.h>
#include <nanobind/make_iterator.h>

void BindProfiler(nb::module_ &m) {
    nb::class_<Profiler, NonCopyable>(m, "Profiler", "Singleton class for managing profiling information")
        .def(nb::init<>())
        .def("next_frame", &Profiler::NextFrame, "Increments the frame counter to provide statistics per frame")
        .def("dump", [](Profiler &self) {
            const char *out;
            self.Dump(out);
            return nb::str(out);
        },
            "Dump profiling statistics at the start of the next frame.\n"
            "Returns:\n"
            "    str: If not empty, this overrides the auto incrementing number in the filename of the dump file")
        .def("add_thread", &Profiler::AddThread, "thread"_a, "Add a thread to be instrumented")
        .def("remove_thread", &Profiler::RemoveThread, "thread"_a, "Remove a thread from being instrumented");

    nb::class_<ProfileSample, NonCopyable>(m, "ProfileSample")
        .def_rw("name", &ProfileSample::mName, "User defined name of this item")
        .def_rw("color", &ProfileSample::mColor, "Color to use for this sample")
        .def_rw("depth", &ProfileSample::mDepth, "Calculated depth")
        .def_prop_rw("unused", [](ProfileSample &self, int index) {
            return self.mUnused[index];
        }, [](ProfileSample &self, int index, uint8_t value) {
            self.mUnused[index] = value;
        })
        .def_rw("start_cycle", &ProfileSample::mStartCycle, "Cycle counter at start of measurement")
        .def_rw("end_cycle", &ProfileSample::mEndCycle, "Cycle counter at end of measurement");

    nb::class_<ProfileThread, NonCopyable>(m, "ProfileThread")
        .def(nb::init<const char *>(), "thread_name"_a, "Constructor")

        .def_ro_static("max_samples", &ProfileThread::cMaxSamples)
        .def_rw("thread_name", &ProfileThread::mThreadName, "Name of the thread that we're collecting information for")
        .def("__len__", [](const ProfileThread & /*pt*/) {
            return ProfileThread::cMaxSamples;
        })
        .def("__getitem__", [](ProfileThread &pt, size_t index) -> ProfileSample & {
            if (index >= ProfileThread::cMaxSamples)
                throw nb::index_error("Sample index out of bounds");

            return pt.mSamples[index];
        }, nb::rv_policy::reference_internal)
        .def("__setitem__", [](ProfileThread &pt, size_t index, const ProfileSample &sample) {
            if (index >= ProfileThread::cMaxSamples)
                throw nb::index_error("Sample index out of bounds");

            // Hack
            memcpy(&pt.mSamples[index], &sample, sizeof(ProfileSample));
        })

        .def("__iter__", [](ProfileThread &pt) {
            return nb::make_iterator(
                nb::type<ProfileThread>(),
                "ProfileSampleIterator",
                &pt.mSamples[0],
                &pt.mSamples[ProfileThread::cMaxSamples]);
        }, nb::keep_alive<0, 1>()) // Nurse (return_value), Patient(self)

        .def_rw("current_sample", &ProfileThread::mCurrentSample, "Next position to write a sample to")

        .def_static("set_instance", &ProfileThread::sSetInstance, "instance"_a)
        .def_static("get_instance", &ProfileThread::sGetInstance)

        .def("__repr__", [](ProfileThread &self) {
            return nb::str(self.mThreadName.c_str());
        });

    nb::class_<ProfileMeasurement, NonCopyable>(m, "ProfileMeasurement", "Create this class on the stack to start sampling timing information of a particular scope")
        .def(nb::init<const char *, uint32>(), "name"_a, "color"_a, "Constructor");
}