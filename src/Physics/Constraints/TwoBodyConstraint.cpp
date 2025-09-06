#include "Common.h"
#include <Jolt/Physics/Constraints/TwoBodyConstraint.h>
#include <Jolt/Physics/IslandBuilder.h>
#include <Jolt/Physics/LargeIslandSplitter.h>
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindTwoBodyConstraint(nb::module_ &m) {
    nb::class_<TwoBodyConstraintSettings, ConstraintSettings> twoBodyConstraintSettingsCls(m, "TwoBodyConstraintSettings",
        "Base class for settings for all constraints that involve 2 bodies");
    twoBodyConstraintSettingsCls
        .def("create", &TwoBodyConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint\n"
            "You can use Body::sFixedToWorld for inBody1 if you want to attach inBody2 to the world");

    nb::class_<TwoBodyConstraint, Constraint> twoBodyConstraintCls(m, "TwoBodyConstraint",
        "Base class for all constraints that involve 2 bodies. Body1 is usually considered the parent, Body2 the child.");
    twoBodyConstraintCls
        // .def(nb::init<Body &, Body &, const TwoBodyConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            // "Constructor")
        .def("get_type", &TwoBodyConstraint::GetType,
            "Get the type of a constraint")
        .def("is_active", &TwoBodyConstraint::IsActive,
            "Solver interface")
        .def("draw_constraint_reference_frame", &TwoBodyConstraint::DrawConstraintReferenceFrame, "renderer"_a)
        .def("get_body1", &TwoBodyConstraint::GetBody1, nb::rv_policy::reference,
            "Access to the connected bodies")
        .def("get_body2", &TwoBodyConstraint::GetBody2, nb::rv_policy::reference)
        .def("get_constraint_to_body1_matrix", &TwoBodyConstraint::GetConstraintToBody1Matrix,
            "Calculates the transform that transforms from constraint space to body 1 space. The first column of the matrix is the primary constraint axis (e.g. the hinge axis / slider direction), second column the secondary etc.")
        .def("get_constraint_to_body2_matrix", &TwoBodyConstraint::GetConstraintToBody2Matrix,
            "Calculates the transform that transforms from constraint space to body 2 space. The first column of the matrix is the primary constraint axis (e.g. the hinge axis / slider direction), second column the secondary etc.")
        .def("build_islands", &TwoBodyConstraint::BuildIslands, "constraint_index"_a, "builder"_a, "body_manager"_a,
            "Link bodies that are connected by this constraint in the island builder")
        .def("build_island_splits", &TwoBodyConstraint::BuildIslandSplits, "splitter"_a,
            "Link bodies that are connected by this constraint in the same split. Returns the split index.");
}