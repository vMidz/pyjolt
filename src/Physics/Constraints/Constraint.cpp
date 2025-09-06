#include "Common.h"
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/LargeIslandSplitter.h>
#include <Jolt/Physics/IslandBuilder.h>
#include <Jolt/Physics/Body/BodyManager.h>

void BindConstraint(nb::module_ &m) {
    nb::enum_<EConstraintType>(m, "EConstraintType",
        "Enum to identify constraint type")
        .value("CONSTRAINT", EConstraintType::Constraint)
        .value("TWO_BODY_CONSTRAINT", EConstraintType::TwoBodyConstraint);
    nb::enum_<EConstraintSubType>(m, "EConstraintSubType",
            "Enum to identify constraint sub type")
        .value("FIXED", EConstraintSubType::Fixed)
        .value("POINT", EConstraintSubType::Point)
        .value("HINGE", EConstraintSubType::Hinge)
        .value("SLIDER", EConstraintSubType::Slider)
        .value("DISTANCE", EConstraintSubType::Distance)
        .value("CONE", EConstraintSubType::Cone)
        .value("SWING_TWIST", EConstraintSubType::SwingTwist)
        .value("SIX_DOF", EConstraintSubType::SixDOF)
        .value("PATH", EConstraintSubType::Path)
        .value("VEHICLE", EConstraintSubType::Vehicle)
        .value("RACK_AND_PINION", EConstraintSubType::RackAndPinion)
        .value("GEAR", EConstraintSubType::Gear)
        .value("PULLEY", EConstraintSubType::Pulley)
        .value("USER1", EConstraintSubType::User1,
            "User defined constraint types start here")
        .value("USER2", EConstraintSubType::User2)
        .value("USER3", EConstraintSubType::User3)
        .value("USER4", EConstraintSubType::User4);
    nb::enum_<EConstraintSpace>(m, "EConstraintSpace",
        "Certain constraints support setting them up in local or world space. This governs what is used.")
        .value("LOCAL_TO_BODY_COM", EConstraintSpace::LocalToBodyCOM,
            "All constraint properties are specified in local space to center of mass of the bodies that are being constrained (so e.g. 'constraint position 1' will be local to body 1 COM, 'constraint position 2' will be local to body 2 COM). Note that this means you need to subtract Shape::GetCenterOfMass() from positions!")
        .value("WORLD_SPACE", EConstraintSpace::WorldSpace,
            "All constraint properties are specified in world space");
    nb::class_<ConstraintSettings, RefTarget<ConstraintSettings>> constraintSettingsCls(m, "ConstraintSettings",
        nb::intrusive_ptr<ConstraintSettings>([](ConstraintSettings *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Class used to store the configuration of a constraint. Allows run-time creation of constraints.");
    constraintSettingsCls
        .def("save_binary_state", &ConstraintSettings::SaveBinaryState, "stream"_a,
            "Saves the contents of the constraint settings in binary form to inStream.")
        .def_static("restore_from_binary_state", &ConstraintSettings::sRestoreFromBinaryState, "stream"_a,
            "Creates a constraint of the correct type and restores its contents from the binary stream inStream.")
        .def_rw("enabled", &ConstraintSettings::mEnabled,
            "If this constraint is enabled initially. Use Constraint::SetEnabled to toggle after creation.")
        .def_rw("constraint_priority", &ConstraintSettings::mConstraintPriority,
            "Priority of the constraint when solving. Higher numbers have are more likely to be solved correctly.\n"
            "Note that if you want a deterministic simulation and you cannot guarantee the order in which constraints are added/removed, you can make the priority for all constraints unique to get a deterministic ordering.")
        .def_rw("num_velocity_steps_override", &ConstraintSettings::mNumVelocityStepsOverride,
            "Used only when the constraint is active. Override for the number of solver velocity iterations to run, 0 means use the default in PhysicsSettings::mNumVelocitySteps. The number of iterations to use is the max of all contacts and constraints in the island.")
        .def_rw("num_position_steps_override", &ConstraintSettings::mNumPositionStepsOverride,
            "Used only when the constraint is active. Override for the number of solver position iterations to run, 0 means use the default in PhysicsSettings::mNumPositionSteps. The number of iterations to use is the max of all contacts and constraints in the island.")
        .def_rw("draw_constraint_size", &ConstraintSettings::mDrawConstraintSize,
            "Size of constraint when drawing it through the debug renderer")
        .def_rw("user_data", &ConstraintSettings::mUserData,
            "User data value (can be used by application)");

    nb::class_<Constraint, RefTarget<Constraint>> constraintCls(m, "Constraint",
        nb::intrusive_ptr<Constraint>([](Constraint *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Base class for all physics constraints. A constraint removes one or more degrees of freedom for a rigid body.");
    constraintCls
        // .def(nb::init<const ConstraintSettings &>(), "settings"_a,
            // "Constructor")
        .def("get_type", &Constraint::GetType,
            "Get the type of a constraint")
        .def("get_sub_type", &Constraint::GetSubType,
            "Get the sub type of a constraint")
        .def("get_constraint_priority", &Constraint::GetConstraintPriority,
            "Priority of the constraint when solving. Higher numbers have are more likely to be solved correctly.\n"
            "Note that if you want a deterministic simulation and you cannot guarantee the order in which constraints are added/removed, you can make the priority for all constraints unique to get a deterministic ordering.")
        .def("set_constraint_priority", &Constraint::SetConstraintPriority, "priority"_a)
        .def("set_num_velocity_steps_override", &Constraint::SetNumVelocityStepsOverride, "n"_a,
            "Used only when the constraint is active. Override for the number of solver velocity iterations to run, 0 means use the default in PhysicsSettings::mNumVelocitySteps. The number of iterations to use is the max of all contacts and constraints in the island.")
        .def("get_num_velocity_steps_override", &Constraint::GetNumVelocityStepsOverride)
        .def("set_num_position_steps_override", &Constraint::SetNumPositionStepsOverride, "n"_a,
            "Used only when the constraint is active. Override for the number of solver position iterations to run, 0 means use the default in PhysicsSettings::mNumPositionSteps. The number of iterations to use is the max of all contacts and constraints in the island.")
        .def("get_num_position_steps_override", &Constraint::GetNumPositionStepsOverride)
        .def("set_enabled", &Constraint::SetEnabled, "enabled"_a,
            "Enable / disable this constraint. This can e.g. be used to implement a breakable constraint by detecting that the constraint impulse\n"
            "(see e.g. PointConstraint::GetTotalLambdaPosition) went over a certain limit and then disabling the constraint.\n"
            "Note that although a disabled constraint will not affect the simulation in any way anymore, it does incur some processing overhead.\n"
            "Alternatively you can remove a constraint from the constraint manager (which may be more costly if you want to disable the constraint for a short while).")
        .def("get_enabled", &Constraint::GetEnabled,
            "Test if a constraint is enabled.")
        .def("get_user_data", &Constraint::GetUserData,
            "Access to the user data, can be used for anything by the application")
        .def("set_user_data", &Constraint::SetUserData, "user_data"_a)
        .def("notify_shape_changed", &Constraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a,
            "Notify the constraint that the shape of a body has changed and that its center of mass has moved by inDeltaCOM.\n"
            "Bodies don't know which constraints are connected to them so the user is responsible for notifying the relevant constraints when a body changes.\n"
            "Args:\n"
            "    body_id (BodyID): ID of the body that has changed.\n"
            "    delta_com (Vec3): The delta of the center of mass of the body (shape->GetCenterOfMass() - shape_before_change->GetCenterOfMass()).")
        .def("reset_warm_start", &Constraint::ResetWarmStart,
            "Notify the system that the configuration of the bodies and/or constraint has changed enough so that the warm start impulses should not be applied the next frame.\n"
            "You can use this function for example when repositioning a ragdoll through Ragdoll::SetPose in such a way that the orientation of the bodies completely changes so that\n"
            "the previous frame impulses are no longer a good approximation of what the impulses will be in the next frame. Calling this function when there are no big changes\n"
            "will result in the constraints being much 'softer' than usual so they are more easily violated (e.g. a long chain of bodies might sag a bit if you call this every frame).")
        .def("is_active", &Constraint::IsActive,
            "///@name Solver interface\n"
            "///@{")
        .def("setup_velocity_constraint", &Constraint::SetupVelocityConstraint, "delta_time"_a)
        .def("warm_start_velocity_constraint", &Constraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &Constraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &Constraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("build_islands", &Constraint::BuildIslands, "constraint_index"_a, "builder"_a, "body_manager"_a,
            "Link bodies that are connected by this constraint in the island builder")
        .def("build_island_splits", &Constraint::BuildIslandSplits, "splitter"_a,
            "Link bodies that are connected by this constraint in the same split. Returns the split index.")
        .def("draw_constraint", &Constraint::DrawConstraint, "renderer"_a)
        .def("draw_constraint_limits", &Constraint::DrawConstraintLimits, "renderer"_a)
        .def("draw_constraint_reference_frame", &Constraint::DrawConstraintReferenceFrame, "renderer"_a)
        .def("get_draw_constraint_size", &Constraint::GetDrawConstraintSize,
            "Size of constraint when drawing it through the debug renderer")
        .def("set_draw_constraint_size", &Constraint::SetDrawConstraintSize, "size"_a)
        .def("save_state", &Constraint::SaveState, "stream"_a,
            "Saving state for replay")
        .def("restore_state", &Constraint::RestoreState, "stream"_a,
            "Restoring state for replay")
        .def("get_constraint_settings", &Constraint::GetConstraintSettings,
            "Debug function to convert a constraint to its settings, note that this will not save to which bodies the constraint is connected to");
}