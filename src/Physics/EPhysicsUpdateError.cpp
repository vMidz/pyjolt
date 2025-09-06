#include "Common.h"
#include <Jolt/Physics/EPhysicsUpdateError.h>

void BindEPhysicsUpdateError(nb::module_ &m) {
    nb::enum_<EPhysicsUpdateError>(m, "EPhysicsUpdateError",
        nb::is_arithmetic(),
        "Enum used by PhysicsSystem to report error conditions during the PhysicsSystem::Update call. This is a bit field, multiple errors can trigger in the same update.")
        .value("NONE", EPhysicsUpdateError::None,
            "No errors")
        .value("MANIFOLD_CACHE_FULL", EPhysicsUpdateError::ManifoldCacheFull,
            "The manifold cache is full, this means that the total number of contacts between bodies is too high. Some contacts were ignored. Increase inMaxContactConstraints in PhysicsSystem::Init.")
        .value("BODY_PAIR_CACHE_FULL", EPhysicsUpdateError::BodyPairCacheFull,
            "The body pair cache is full, this means that too many bodies contacted. Some contacts were ignored. Increase inMaxBodyPairs in PhysicsSystem::Init.")
        .value("CONTACT_CONSTRAINTS_FULL", EPhysicsUpdateError::ContactConstraintsFull,
            "The contact constraints buffer is full. Some contacts were ignored. Increase inMaxContactConstraints in PhysicsSystem::Init.");
}