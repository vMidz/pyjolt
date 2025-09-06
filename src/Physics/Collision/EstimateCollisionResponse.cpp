#include "Common.h"
#include <Jolt/Physics/Collision/EstimateCollisionResponse.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>

void BindEstimateCollisionResponse(nb::module_ &m) {
    m.def("estimate_collision_response", &EstimateCollisionResponse, "body1"_a, "body2"_a, "manifold"_a, "result"_a, "combined_friction"_a, "combined_restitution"_a, "min_velocity_for_restitution"_a = 1.0f, "num_iterations"_a = 10,
        "This function estimates the contact impulses and body velocity changes as a result of a collision.\n"
        "It can be used in the ContactListener::OnContactAdded to determine the strength of the collision to e.g. play a sound or trigger a particle system.\n"
        "This function is accurate when two bodies collide but will not be accurate when more than 2 bodies collide at the same time as it does not know about these other collisions.\n"
        "Args:\n"
        "    body1 (Body): Colliding body 1.\n"
        "    body2 (Body): Colliding body 2.\n"
        "    manifold (ContactManifold): The collision manifold.\n"
        "    result (CollisionEstimationResult): A structure that contains the estimated contact and friction impulses and the resulting body velocities.\n"
        "    combined_friction (float): The combined friction of body 1 and body 2 (see ContactSettings::mCombinedFriction).\n"
        "    combined_restitution (float): The combined restitution of body 1 and body 2 (see ContactSettings::mCombinedRestitution).\n"
        "    min_velocity_for_restitution (float): Minimal velocity required for restitution to be applied (see PhysicsSettings::mMinVelocityForRestitution).\n"
        "    num_iterations (int): Number of iterations to use for the impulse estimation (see PhysicsSettings::mNumVelocitySteps, note you can probably use a lower number for a decent estimate). If you set the number of iterations to 1 then no friction will be calculated.");

    nb::class_<CollisionEstimationResult> collisionEstimationResultCls(m, "CollisionEstimationResult",
        "A structure that contains the estimated contact and friction impulses and the resulting body velocities");
    collisionEstimationResultCls
        .def(nb::init<>())
        .def_rw("linear_velocity1", &CollisionEstimationResult::mLinearVelocity1,
            "The estimated linear velocity of body 1 after collision")
        .def_rw("angular_velocity1", &CollisionEstimationResult::mAngularVelocity1,
            "The estimated angular velocity of body 1 after collision")
        .def_rw("linear_velocity2", &CollisionEstimationResult::mLinearVelocity2,
            "The estimated linear velocity of body 2 after collision")
        .def_rw("angular_velocity2", &CollisionEstimationResult::mAngularVelocity2,
            "The estimated angular velocity of body 2 after collision")
        .def_rw("tangent1", &CollisionEstimationResult::mTangent1,
            "Normalized tangent of contact normal")
        .def_rw("tangent2", &CollisionEstimationResult::mTangent2,
            "Second normalized tangent of contact normal (forms a basis with mTangent1 and mWorldSpaceNormal)")
        .def_rw("impulses", &CollisionEstimationResult::mImpulses);

    nb::class_<CollisionEstimationResult::Impulse> impulseCls(collisionEstimationResultCls, "Impulse");
    impulseCls
        .def_rw("contact_impulse", &CollisionEstimationResult::Impulse::mContactImpulse,
            "Estimated contact impulses (kg m / s)")
        .def_rw("friction_impulse1", &CollisionEstimationResult::Impulse::mFrictionImpulse1,
            "Estimated friction impulses in the direction of tangent 1 (kg m / s)")
        .def_rw("friction_impulse2", &CollisionEstimationResult::Impulse::mFrictionImpulse2,
            "Estimated friction impulses in the direction of tangent 2 (kg m / s)");

}