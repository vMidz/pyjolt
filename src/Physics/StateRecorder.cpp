#include "Common.h"
#include <Jolt/Physics/StateRecorder.h>
#include <nanobind/trampoline.h>
#include <nanobind/operators.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Constraints/Constraint.h>

class PyStateRecorderFilter : public StateRecorderFilter {
  public:
    NB_TRAMPOLINE(StateRecorderFilter, 4);

    ~PyStateRecorderFilter() override = default;

    bool ShouldSaveBody(const Body &inBody) const override {
        NB_OVERRIDE_NAME(
            "should_save_body",
            ShouldSaveBody,
            inBody);
    }

    bool ShouldSaveConstraint(const Constraint &inConstraint) const override {
        NB_OVERRIDE_NAME(
            "should_save_constraint",
            ShouldSaveConstraint,
            inConstraint);
    }

    bool ShouldSaveContact(const BodyID &inBody1, const BodyID &inBody2) const override {
        NB_OVERRIDE_NAME(
            "should_save_contact",
            ShouldSaveContact,
            inBody1,
            inBody2);
    }

    bool ShouldRestoreContact(const BodyID &inBody1, const BodyID &inBody2) const override {
        NB_OVERRIDE_NAME(
            "should_restore_contact",
            ShouldRestoreContact,
            inBody1,
            inBody2);
    }
};

void BindStateRecorder(nb::module_ &m) {
    nb::enum_<EStateRecorderState>(m, "EStateRecorderState",
        nb::is_arithmetic(),
        "A bit field that determines which aspects of the simulation to save")
        .value("NONE", EStateRecorderState::None,
            "Save nothing")
        .value("GLOBAL", EStateRecorderState::Global,
            "Save global physics system state (delta time, gravity, etc.)")
        .value("BODIES", EStateRecorderState::Bodies,
            "Save the state of bodies")
        .value("CONTACTS", EStateRecorderState::Contacts,
            "Save the state of contacts")
        .value("CONSTRAINTS", EStateRecorderState::Constraints,
            "Save the state of constraints")
        .value("ALL", EStateRecorderState::All,
            "Save all state");

    nb::class_<StateRecorderFilter> stateRecorderFilterCls(m, "StateRecorderFilter",
        "User callbacks that allow determining which parts of the simulation should be saved by a StateRecorder");
    stateRecorderFilterCls
        .def("should_save_body", &StateRecorderFilter::ShouldSaveBody, "body"_a,
            "If the state of a specific body should be saved")
        .def("should_save_constraint", &StateRecorderFilter::ShouldSaveConstraint, "constraint"_a,
            "If the state of a specific constraint should be saved")
        .def("should_save_contact", &StateRecorderFilter::ShouldSaveContact, "body1"_a, "body2"_a,
            "If the state of a specific contact should be saved")
        .def("should_restore_contact", &StateRecorderFilter::ShouldRestoreContact, "body1"_a, "body2"_a,
            "If the state of a specific contact should be restored");

    nb::class_<StateRecorder, StreamIn> stateRecorderCls(m, "StateRecorder",
        "Class that records the state of a physics system. Can be used to check if the simulation is deterministic by putting the recorder in validation mode.\n"
        "Can be used to restore the state to an earlier point in time. Note that only the state that is modified by the simulation is saved, configuration settings\n"
        "like body friction or restitution, motion quality etc. are not saved and need to be saved by the user if desired.");
    stateRecorderCls
        // .def(nb::init<>(),       // Abstract class
            // "Constructor")
        // .def(nb::init<const StateRecorder &>(), "rhs"_a)
        .def("set_validating", &StateRecorder::SetValidating, "validating"_a,
            "Sets the stream in validation mode. In this case the physics system ensures that before it calls ReadBytes that it will\n"
            "ensure that those bytes contain the current state. This makes it possible to step and save the state, restore to the previous\n"
            "step and step again and when the recorded state is not the same it can restore the expected state and any byte that changes\n"
            "due to a ReadBytes function can be caught to find out which part of the simulation is not deterministic.\n"
            "Note that validation only works when saving the full state of the simulation (EStateRecorderState::All, StateRecorderFilter == nullptr).")
        .def("is_validating", &StateRecorder::IsValidating)
        .def("set_is_last_part", &StateRecorder::SetIsLastPart, "is_last_part"_a,
            "This allows splitting the state in multiple parts. While restoring, only the last part should have this flag set to true.\n"
            "Note that you should ensure that the different parts contain information for disjoint sets of bodies, constraints and contacts.\n"
            "E.g. if you restore the same contact twice, you get undefined behavior. In order to create disjoint sets you can use the StateRecorderFilter.\n"
            "Note that validation is not compatible with restoring a simulation state in multiple parts.")
        .def("is_last_part", &StateRecorder::IsLastPart);
}