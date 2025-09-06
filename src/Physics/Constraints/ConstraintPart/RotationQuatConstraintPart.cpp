#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/RotationQuatConstraintPart.h>

void BindRotationQuatConstraintPart(nb::module_ &m) {
    nb::class_<RotationQuatConstraintPart> rotationQuatConstraintPartCls(m, "RotationQuatConstraintPart",
        "Quaternion based constraint that constrains rotation around all axis so that only translation is allowed.\n"
        "NOTE: This constraint part is more expensive than the RotationEulerConstraintPart and slightly more correct since\n"
        "RotationEulerConstraintPart::SolvePositionConstraint contains an approximation. In practice the difference\n"
        "is small, so the RotationEulerConstraintPart is probably the better choice.\n"
        "Rotation is fixed between bodies like this:\n"
        "q2 = q1 r0\n"
        "Where:\n"
        "q1, q2 = world space quaternions representing rotation of body 1 and 2.\n"
        "r0 = initial rotation between bodies in local space of body 1, this can be calculated by:\n"
        "q20 = q10 r0\n"
        "<=> r0 = q10^* q20\n"
        "Where:\n"
        "q10, q20 = initial world space rotations of body 1 and 2.\n"
        "q10^* = conjugate of quaternion q10 (which is the same as the inverse for a unit quaternion)\n"
        "We exclusively use the conjugate below:\n"
        "r0^* = q20^* q10\n"
        "The error in the rotation is (in local space of body 1):\n"
        "q2 = q1 error r0\n"
        "<=> error = q1^* q2 r0^*\n"
        "The imaginary part of the quaternion represents the rotation axis * sin(angle / 2). The real part of the quaternion\n"
        "does not add any additional information (we know the quaternion in normalized) and we're removing 3 degrees of freedom\n"
        "so we want 3 parameters. Therefore we define the constraint equation like:\n"
        "C = A q1^* q2 r0^* = 0\n"
        "Where (if you write a quaternion as [real-part, i-part, j-part, k-part]):\n"
        "        [0, 1, 0, 0]\n"
        "    A = [0, 0, 1, 0]\n"
        "        [0, 0, 0, 1]\n"
        "or in our case since we store a quaternion like [i-part, j-part, k-part, real-part]:\n"
        "        [1, 0, 0, 0]\n"
        "    A = [0, 1, 0, 0]\n"
        "        [0, 0, 1, 0]\n"
        "Time derivative:\n"
        "d/dt C = A (q1^* d/dt(q2) + d/dt(q1^*) q2) r0^*\n"
        "= A (q1^* (1/2 W2 q2) + (1/2 W1 q1)^* q2) r0^*\n"
        "= 1/2 A (q1^* W2 q2 + q1^* W1^* q2) r0^*\n"
        "= 1/2 A (q1^* W2 q2 - q1^* W1 * q2) r0^*\n"
        "= 1/2 A ML(q1^*) MR(q2 r0^*) (W2 - W1)\n"
        "= 1/2 A ML(q1^*) MR(q2 r0^*) A^T (w2 - w1)\n"
        "Where:\n"
        "W1 = [0, w1], W2 = [0, w2] (converting angular velocity to imaginary part of quaternion).\n"
        "w1, w2 = angular velocity of body 1 and 2.\n"
        "d/dt(q) = 1/2 W q (time derivative of a quaternion).\n"
        "W^* = -W (conjugate negates angular velocity as quaternion).\n"
        "ML(q): 4x4 matrix so that q * p = ML(q) * p, where q and p are quaternions.\n"
        "MR(p): 4x4 matrix so that q * p = MR(p) * q, where q and p are quaternions.\n"
        "A^T: Transpose of A.\n"
        "Jacobian:\n"
        "J = [0, -1/2 A ML(q1^*) MR(q2 r0^*) A^T, 0, 1/2 A ML(q1^*) MR(q2 r0^*) A^T]\n"
        "= [0, -JP, 0, JP]\n"
        "Suggested reading:\n"
        "- 3D Constraint Derivations for Impulse Solvers - Marijn Tamis\n"
        "- Game Physics Pearls - Section 9 - Quaternion Based Constraints - Claude Lacoursiere");
    rotationQuatConstraintPartCls
        .def_static("get_inv_initial_orientation", &RotationQuatConstraintPart::sGetInvInitialOrientation, "body1"_a, "body2"_a,
            "Return inverse of initial rotation from body 1 to body 2 in body 1 space")
        .def("calculate_constraint_properties", &RotationQuatConstraintPart::CalculateConstraintProperties, "body1"_a, "rotation1"_a, "body2"_a, "rotation2"_a, "inv_initial_orientation"_a,
            "Calculate properties used during the functions below")
        .def("deactivate", &RotationQuatConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &RotationQuatConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &RotationQuatConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses")
        .def("solve_velocity_constraint", &RotationQuatConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.")
        .def("solve_position_constraint", &RotationQuatConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "inv_initial_orientation"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.")
        .def("get_total_lambda", &RotationQuatConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("save_state", &RotationQuatConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &RotationQuatConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}