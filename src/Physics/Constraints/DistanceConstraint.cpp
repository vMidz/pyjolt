#include "Common.h"
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindDistanceConstraint(nb::module_ &m) {
    nb::class_<DistanceConstraintSettings, TwoBodyConstraintSettings> distanceConstraintSettingsCls(m, "DistanceConstraintSettings",
        "Distance constraint settings, used to create a distance constraint",
        nb::is_final());
    distanceConstraintSettingsCls
        .def(nb::init<>())
        .def("save_binary_state", &DistanceConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &DistanceConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("space", &DistanceConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("point1", &DistanceConstraintSettings::mPoint1,
            "Body 1 constraint reference frame (space determined by mSpace).\n"
            "Constraint will keep mPoint1 (a point on body 1) and mPoint2 (a point on body 2) at the same distance.\n"
            "Note that this constraint can be used as a cheap PointConstraint by setting mPoint1 = mPoint2 (but this removes only 1 degree of freedom instead of 3).")
        .def_rw("point2", &DistanceConstraintSettings::mPoint2,
            "Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("min_distance", &DistanceConstraintSettings::mMinDistance,
            "Ability to override the distance range at which the two points are kept apart. If the value is negative, it will be replaced by the distance between mPoint1 and mPoint2 (works only if mSpace is world space).")
        .def_rw("max_distance", &DistanceConstraintSettings::mMaxDistance)
        .def_rw("limits_spring_settings", &DistanceConstraintSettings::mLimitsSpringSettings,
            "When enabled, this makes the limits soft. When the constraint exceeds the limits, a spring force will pull it back.");

    nb::class_<DistanceConstraint, TwoBodyConstraint> distanceConstraintCls(m, "DistanceConstraint",
        "This constraint is a stiff spring that holds 2 points at a fixed distance from each other",
        nb::is_final());
    distanceConstraintCls
        .def(nb::init<Body &, Body &, const DistanceConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct distance constraint")
        .def("get_sub_type", &DistanceConstraint::GetSubType)
        .def("notify_shape_changed", &DistanceConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &DistanceConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &DistanceConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &DistanceConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &DistanceConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &DistanceConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &DistanceConstraint::DrawConstraint, "renderer"_a)
        .def("save_state", &DistanceConstraint::SaveState, "stream"_a)
        .def("restore_state", &DistanceConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &DistanceConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &DistanceConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &DistanceConstraint::GetConstraintToBody2Matrix)
        .def("set_distance", &DistanceConstraint::SetDistance, "min_distance"_a, "max_distance"_a,
            "Update the minimum and maximum distance for the constraint")
        .def("get_min_distance", &DistanceConstraint::GetMinDistance)
        .def("get_max_distance", &DistanceConstraint::GetMaxDistance)
        .def("get_limits_spring_settings", nb::overload_cast<>(&DistanceConstraint::GetLimitsSpringSettings),
            "Update the limits spring settings")
        .def("set_limits_spring_settings", &DistanceConstraint::SetLimitsSpringSettings, "limits_spring_settings"_a)
        .def("get_total_lambda_position", &DistanceConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear impulse applied to satisfy the constraint)");
}