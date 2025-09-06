#include "Common.h"
#include <Jolt/Physics/Vehicle/Wheel.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>

void BindWheel(nb::module_ &m) {
    nb::class_<WheelSettings, RefTarget<WheelSettings>> wheelSettingsCls(m, "WheelSettings",
        nb::intrusive_ptr<WheelSettings>([](WheelSettings *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Base class for wheel settings, each VehicleController can implement a derived class of this");
    wheelSettingsCls
        .def("save_binary_state", &WheelSettings::SaveBinaryState, "stream"_a,
            "Saves the contents in binary form to inStream.")
        .def("restore_binary_state", &WheelSettings::RestoreBinaryState, "stream"_a,
            "Restores the contents in binary form to inStream.")
        .def_rw("position", &WheelSettings::mPosition,
            "Attachment point of wheel suspension in local space of the body")
        .def_rw("suspension_force_point", &WheelSettings::mSuspensionForcePoint,
            "Where tire forces (suspension and traction) are applied, in local space of the body. A good default is the center of the wheel in its neutral pose. See mEnableSuspensionForcePoint.")
        .def_rw("suspension_direction", &WheelSettings::mSuspensionDirection,
            "Direction of the suspension in local space of the body, should point down")
        .def_rw("steering_axis", &WheelSettings::mSteeringAxis,
            "Direction of the steering axis in local space of the body, should point up (e.g. for a bike would be -mSuspensionDirection)")
        .def_rw("wheel_up", &WheelSettings::mWheelUp,
            "Up direction when the wheel is in the neutral steering position (usually VehicleConstraintSettings::mUp but can be used to give the wheel camber or for a bike would be -mSuspensionDirection)")
        .def_rw("wheel_forward", &WheelSettings::mWheelForward,
            "Forward direction when the wheel is in the neutral steering position (usually VehicleConstraintSettings::mForward but can be used to give the wheel toe, does not need to be perpendicular to mWheelUp)")
        .def_rw("suspension_min_length", &WheelSettings::mSuspensionMinLength,
            "How long the suspension is in max raised position relative to the attachment point (m)")
        .def_rw("suspension_max_length", &WheelSettings::mSuspensionMaxLength,
            "How long the suspension is in max droop position relative to the attachment point (m)")
        .def_rw("suspension_preload_length", &WheelSettings::mSuspensionPreloadLength,
            "The natural length (m) of the suspension spring is defined as mSuspensionMaxLength + mSuspensionPreloadLength. Can be used to preload the suspension as the spring is compressed by mSuspensionPreloadLength when the suspension is in max droop position. Note that this means when the vehicle touches the ground there is a discontinuity so it will also make the vehicle more bouncy as we're updating with discrete time steps.")
        .def_rw("suspension_spring", &WheelSettings::mSuspensionSpring,
            "Settings for the suspension spring")
        .def_rw("radius", &WheelSettings::mRadius,
            "Radius of the wheel (m)")
        .def_rw("width", &WheelSettings::mWidth,
            "Width of the wheel (m)")
        .def_rw("enable_suspension_force_point", &WheelSettings::mEnableSuspensionForcePoint,
            "Enables mSuspensionForcePoint, if disabled, the forces are applied at the collision contact point. This leads to a more accurate simulation when interacting with dynamic objects but makes the vehicle less stable. When setting this to true, all forces will be applied to a fixed point on the vehicle body.");

    nb::class_<Wheel, NonCopyable> wheelCls(m, "Wheel",
        "Base class for runtime data for a wheel, each VehicleController can implement a derived class of this");
    wheelCls
        .def(nb::init<const WheelSettings &>(), "settings"_a,
            "Constructor / destructor")
        .def("get_settings", &Wheel::GetSettings, nb::rv_policy::reference,
            "Get settings for the wheel")
        .def("get_angular_velocity", &Wheel::GetAngularVelocity,
            "Get the angular velocity (rad/s) for this wheel, note that positive means the wheel is rotating such that the car moves forward")
        .def("set_angular_velocity", &Wheel::SetAngularVelocity, "vel"_a,
            "Update the angular velocity (rad/s)")
        .def("get_rotation_angle", &Wheel::GetRotationAngle,
            "Get the current rotation angle of the wheel in radians [0, 2 pi]")
        .def("set_rotation_angle", &Wheel::SetRotationAngle, "angle"_a,
            "Set the current rotation angle of the wheel in radians [0, 2 pi]")
        .def("get_steer_angle", &Wheel::GetSteerAngle,
            "Get the current steer angle of the wheel in radians [-pi, pi], positive is to the left")
        .def("set_steer_angle", &Wheel::SetSteerAngle, "angle"_a,
            "Set the current steer angle of the wheel in radians [-pi, pi]")
        .def("has_contact", &Wheel::HasContact,
            "Returns true if the wheel is touching an object")
        .def("get_contact_body_id", &Wheel::GetContactBodyID,
            "Returns the body ID of the body that this wheel is touching")
        .def("get_contact_sub_shape_id", &Wheel::GetContactSubShapeID,
            "Returns the sub shape ID where we're contacting the body")
        .def("get_contact_position", &Wheel::GetContactPosition,
            "Returns the current contact position in world space (note by the time you call this the vehicle has moved)")
        .def("get_contact_point_velocity", &Wheel::GetContactPointVelocity,
            "Velocity of the contact point (m / s, not relative to the wheel but in world space)")
        .def("get_contact_normal", &Wheel::GetContactNormal,
            "Returns the current contact normal in world space (note by the time you call this the vehicle has moved)")
        .def("get_contact_longitudinal", &Wheel::GetContactLongitudinal,
            "Returns longitudinal direction (direction along the wheel relative to floor) in world space (note by the time you call this the vehicle has moved)")
        .def("get_contact_lateral", &Wheel::GetContactLateral,
            "Returns lateral direction (sideways direction) in world space (note by the time you call this the vehicle has moved)")
        .def("get_suspension_length", &Wheel::GetSuspensionLength,
            "Get the length of the suspension for a wheel (m) relative to the suspension attachment point (hard point)")
        .def("has_hit_hard_point", &Wheel::HasHitHardPoint,
            "Check if the suspension hit its upper limit")
        .def("get_suspension_lambda", &Wheel::GetSuspensionLambda,
            "Get the total impulse (N s) that was applied by the suspension")
        .def("get_longitudinal_lambda", &Wheel::GetLongitudinalLambda,
            "Get total impulse (N s) applied along the forward direction of the wheel")
        .def("get_lateral_lambda", &Wheel::GetLateralLambda,
            "Get total impulse (N s) applied along the sideways direction of the wheel")
        .def("solve_longitudinal_constraint_part", &Wheel::SolveLongitudinalConstraintPart, "constraint"_a, "min_impulse"_a, "max_impulse"_a,
            "Internal function that should only be called by the controller. Used to apply impulses in the forward direction of the vehicle.")
        .def("solve_lateral_constraint_part", &Wheel::SolveLateralConstraintPart, "constraint"_a, "min_impulse"_a, "max_impulse"_a,
            "Internal function that should only be called by the controller. Used to apply impulses in the sideways direction of the vehicle.");
}