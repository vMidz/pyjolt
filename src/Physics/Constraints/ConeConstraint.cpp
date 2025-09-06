#include "Common.h"
#include <Jolt/Physics/Constraints/ConeConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindConeConstraint(nb::module_ &m) {
    nb::class_<ConeConstraintSettings, TwoBodyConstraintSettings> coneConstraintSettingsCls(m, "ConeConstraintSettings",
        "Cone constraint settings, used to create a cone constraint",
        nb::is_final());
    coneConstraintSettingsCls
        .def("save_binary_state", &ConeConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &ConeConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("space", &ConeConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("point1", &ConeConstraintSettings::mPoint1,
            "Body 1 constraint reference frame (space determined by mSpace)")
        .def_rw("twist_axis1", &ConeConstraintSettings::mTwistAxis1)
        .def_rw("point2", &ConeConstraintSettings::mPoint2,
            "Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("twist_axis2", &ConeConstraintSettings::mTwistAxis2)
        .def_rw("half_cone_angle", &ConeConstraintSettings::mHalfConeAngle,
            "Half of maximum angle between twist axis of body 1 and 2");

    nb::class_<ConeConstraint, TwoBodyConstraint> coneConstraintCls(m, "ConeConstraint",
        "A cone constraint constraints 2 bodies to a single point and limits the swing between the twist axis within a cone:\n"
        "t1 . t2 <= cos(theta)\n"
        "Where:\n"
        "t1 = twist axis of body 1.\n"
        "t2 = twist axis of body 2.\n"
        "theta = half cone angle (angle from the principal axis of the cone to the edge).\n"
        "Calculating the Jacobian:\n"
        "Constraint equation:\n"
        "C = t1 . t2 - cos(theta)\n"
        "Derivative:\n"
        "d/dt C = d/dt (t1 . t2) = (d/dt t1) . t2 + t1 . (d/dt t2) = (w1 x t1) . t2 + t1 . (w2 x t2) = (t1 x t2) . w1 + (t2 x t1) . w2\n"
        "d/dt C = J v = [0, -t2 x t1, 0, t2 x t1] [v1, w1, v2, w2]\n"
        "Where J is the Jacobian.\n"
        "Note that this is the exact same equation as used in AngleConstraintPart if we use t2 x t1 as the world space axis",
        nb::is_final());
    coneConstraintCls
        .def(nb::init<Body &, Body &, const ConeConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct cone constraint")
        .def("get_sub_type", &ConeConstraint::GetSubType)
        .def("notify_shape_changed", &ConeConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &ConeConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &ConeConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &ConeConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &ConeConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &ConeConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &ConeConstraint::DrawConstraint, "renderer"_a)
        .def("draw_constraint_limits", &ConeConstraint::DrawConstraintLimits, "renderer"_a)
        .def("save_state", &ConeConstraint::SaveState, "stream"_a)
        .def("restore_state", &ConeConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &ConeConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &ConeConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &ConeConstraint::GetConstraintToBody2Matrix)
        .def("set_half_cone_angle", &ConeConstraint::SetHalfConeAngle, "half_cone_angle"_a,
            "Update maximum angle between body 1 and 2 (see ConeConstraintSettings)")
        .def("get_cos_half_cone_angle", &ConeConstraint::GetCosHalfConeAngle)
        .def("get_total_lambda_position", &ConeConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)")
        .def("get_total_lambda_rotation", &ConeConstraint::GetTotalLambdaRotation);
}