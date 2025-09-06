#include "Common.h"
#include <Jolt/Physics/SoftBody/SoftBodyVertex.h>

void BindSoftBodyVertex(nb::module_ &m) {
    nb::class_<SoftBodyVertex> softBodyVertexCls(m, "SoftBodyVertex",
        "Run time information for a single particle of a soft body\n"
        "Note that at run-time you should only modify the inverse mass and/or velocity of a vertex to control the soft body.\n"
        "Modifying the position can lead to missed collisions.\n"
        "The other members are used internally by the soft body solver.");
    softBodyVertexCls
        .def("reset_collision", &SoftBodyVertex::ResetCollision,
            "Reset collision information to prepare for a new collision check")
        .def_rw("previous_position", &SoftBodyVertex::mPreviousPosition,
            "Internal use only. Position at the previous time step")
        .def_rw("position", &SoftBodyVertex::mPosition,
            "Position, relative to the center of mass of the soft body")
        .def_rw("velocity", &SoftBodyVertex::mVelocity,
            "Velocity, relative to the center of mass of the soft body")
        .def_rw("collision_plane", &SoftBodyVertex::mCollisionPlane,
            "Internal use only. Nearest collision plane, relative to the center of mass of the soft body")
        .def_rw("colliding_shape_index", &SoftBodyVertex::mCollidingShapeIndex,
            "Internal use only. Index in the colliding shapes list of the body we may collide with")
        .def_rw("has_contact", &SoftBodyVertex::mHasContact,
            "True if the vertex has collided with anything in the last update")
        .def_rw("largest_penetration", &SoftBodyVertex::mLargestPenetration,
            "Internal use only. Used while finding the collision plane, stores the largest penetration found so far")
        .def_rw("inv_mass", &SoftBodyVertex::mInvMass,
            "Inverse mass (1 / mass)");
}