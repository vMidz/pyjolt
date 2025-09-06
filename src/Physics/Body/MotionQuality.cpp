#include "Common.h"
#include <Jolt/Physics/Body/MotionQuality.h>

void BindMotionQuality(nb::module_ &m) {
    nb::enum_<EMotionQuality>(m, "EMotionQuality",
        "Motion quality, or how well it detects collisions when it has a high velocity")
        .value("DISCRETE", EMotionQuality::Discrete,
            "Update the body in discrete steps. Body will tunnel throuh thin objects if its velocity is high enough.\n"
            "This is the cheapest way of simulating a body.")

        .value("LINEAR_CAST", EMotionQuality::LinearCast,
            "Update the body using linear casting. When stepping the body, its collision shape is cast from\n"
            "start to destination using the starting rotation. The body will not be able to tunnel through thin\n"
            "objects at high velocity, but tunneling is still possible if the body is long and thin and has high\n"
            "angular velocity. Time is stolen from the object (which means it will move up to the first collision\n"
            "and will not bounce off the surface until the next integration step). This will make the body appear\n"
            "to go slower when it collides with high velocity. In order to not get stuck, the body is always\n"
            "allowed to move by a fraction of it's inner radius, which may eventually lead it to pass through geometry.\n"
            "Note that if you're using a collision listener, you can receive contact added/persisted notifications of contacts\n"
            "that may in the end not happen. This happens between bodies that are using casting: If bodies A and B collide at t1\n"
            "and B and C collide at t2 where t2 < t1 and A and C don't collide. In this case you may receive an incorrect contact\n"
            "point added callback between A and B (which will be removed the next frame).");
}