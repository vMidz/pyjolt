
#include "Common.h"
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/Body/Body.h>

#include <nanobind/trampoline.h>

class PyBodyFilter : public BodyFilter {
    NB_TRAMPOLINE(BodyFilter, 2);

    ~PyBodyFilter() override {
    }

    bool ShouldCollide([[maybe_unused]] const BodyID &inBodyID) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inBodyID);
    }

    bool ShouldCollideLocked([[maybe_unused]] const Body &inBody) const override {
        NB_OVERRIDE_NAME(
            "should_collide_locked",
            ShouldCollideLocked,
            inBody);
    }
};

void BindBodyFilter(nb::module_ &m) {
    nb::class_<BodyFilter, PyBodyFilter>(m, "BodyFilter",
        "Class function to filter out bodies, returns true if test should collide with body")
        .def(nb::init<>())
        .def("should_collide", &BodyFilter::ShouldCollide, "body_id"_a,
            "Filter function. Returns true if we should collide with inBodyID")
        .def("should_collide_locked", &BodyFilter::ShouldCollideLocked, "body_id"_a,
            "Filter function. Returns true if we should collide with inBody (this is called after the body is locked and makes it possible to filter based on body members)");

    nb::class_<IgnoreSingleBodyFilter, BodyFilter>(m, "IgnoreSingleBodyFilter",
        "A simple body filter implementation that ignores a single, specified body")
        .def(nb::init<const BodyID &>(), "body_id"_a, "Constructor, pass the body you want to ignore")
        .def("should_collide", &IgnoreSingleBodyFilter::ShouldCollide, "body_id"_a,
            "Filter function. Returns true if we should collide with inBodyID");

    nb::class_<IgnoreMultipleBodiesFilter, BodyFilter>(m, "IgnoreMultipleBodiesFilter",
        "A simple body filter implementation that ignores multiple, specified bodies")
        .def("clear", &IgnoreMultipleBodiesFilter::Clear, "Remove all bodies from the filter")
        .def("reserve", &IgnoreMultipleBodiesFilter::Reserve, "size"_a, "Reserve space for inSize body ID's")
        .def("ignore_body", &IgnoreMultipleBodiesFilter::IgnoreBody, "body_id"_a, "Add a body to be ignored")
        .def("should_collide", &IgnoreMultipleBodiesFilter::ShouldCollide, "body_id"_a,
            "Filter function. Returns true if we should collide with inBodyID");

    nb::class_<IgnoreSingleBodyFilterChained, BodyFilter>(m, "IgnoreSingleBodyFilterChained",
        "Ignores a single body and chains the filter to another filter")
        .def(nb::init<const BodyID, const BodyFilter &>(), "body_id"_a, "body_filter"_a, "Constructor")
        .def("should_collide", &IgnoreSingleBodyFilterChained::ShouldCollide, "body_id"_a,
            "Filter function. Returns true if we should collide with inBodyID")
        .def("should_collide_locked", &IgnoreSingleBodyFilterChained::ShouldCollideLocked, "body_id"_a,
            "Filter function. Returns true if we should collide with inBody (this is called after the body is locked and makes it possible to filter based on body members)");

    nb::class_<BodyDrawFilter, NonCopyable>(m, "BodyDrawFilter",
        "Class function to filter out bodies for debug rendering, returns true if body should be rendered")
        .def("should_draw", &BodyDrawFilter::ShouldDraw, "body_id"_a,
            "Filter function. Returns true if inBody should be rendered");
}
