#include "Common.h"
#include <Jolt/Geometry/GJKClosestPoint.h>
#include <Jolt/Geometry/Sphere.h>
#include <Jolt/Geometry/OrientedBox.h>

void BindGJKClosestPoint(nb::module_ &m) {
    nb::class_<GJKClosestPoint, NonCopyable>(m, "GJKClosestPoint",
        "Convex vs convex collision detection\n"
        "Based on: A Fast and Robust GJK Implementation for Collision Detection of Convex Objects - Gino van den Bergen")

    // TODO: Add more shapes?
    .def("intersects", &GJKClosestPoint::Intersects<Sphere, Sphere>, "a"_a, "b"_a, "tolerance"_a, "v"_a,
        "Test if inA and inB intersect.\n"
        "Args:\n"
        "    a (A): The convex object A, must support the GetSupport(Vec3) function.\n"
        "    b (B): The convex object B, must support the GetSupport(Vec3) function.\n"
        "    tolerance (float): Minimal distance between objects when the objects are considered to be colliding.\n"
        "    v (Vec3): is used as initial separating axis (provide a zero vector if you don't know yet).\n"
        "Returns:\n"
        "    bool: True if they intersect (in which case ioV = (0, 0, 0)).\n"
        "False if they don't intersect in which case ioV is a separating axis in the direction from A to B (magnitude is meaningless).")

    .def("get_closest_points", &GJKClosestPoint::GetClosestPoints<Sphere, Sphere>,
        "a"_a, "b"_a, "tolerance"_a, "max_dist_sq"_a, "v"_a, "point_a"_a, "point_b"_a,
        "Get closest points between inA and inB.\n"
        "Args:\n"
        "    a (A): The convex object A, must support the GetSupport(Vec3) function.\n"
        "    b (B): The convex object B, must support the GetSupport(Vec3) function.\n"
        "    tolerance (float): The minimal distance between A and B before the objects are considered colliding and processing is terminated.\n"
        "    max_dist_sq (float): The maximum squared distance between A and B before the objects are considered infinitely far away and processing is terminated.\n"
        "    v (Vec3): Initial guess for the separating axis. Start with any non-zero vector if you don't know.\n"
        "    If return value is 0, ioV = (0, 0, 0).\n"
        "    If the return value is bigger than 0 but smaller than FLT_MAX, ioV will be the separating axis in the direction from A to B and its length the squared distance between A and B.\n"
        "    If the return value is FLT_MAX, ioV will be the separating axis in the direction from A to B and the magnitude of the vector is meaningless.\n"
        "    point_a (Vec3): , outPointB\n"
        "    If the return value is 0 the points are invalid.\n"
        "    If the return value is bigger than 0 but smaller than FLT_MAX these will contain the closest point on A and B.\n"
        "    If the return value is FLT_MAX the points are invalid.\n"
        "Returns:\n"
        "    float: The squared distance between A and B or FLT_MAX when they are further away than inMaxDistSq.")

    .def("get_closest_points_simplex", &GJKClosestPoint::GetClosestPointsSimplex, "y"_a, "p"_a, "q"_a, "num_points"_a,
        "Get the resulting simplex after the GetClosestPoints algorithm finishes.\n"
        "If it returned a squared distance of 0, the origin will be contained in the simplex.")

    .def("cast_ray", &GJKClosestPoint::CastRay<Sphere>, "ray_origin"_a, "ray_direction"_a, "tolerance"_a, "a"_a, "lambda_"_a,
        "Test if a ray inRayOrigin + lambda * inRayDirection for lambda e [0, ioLambda> intersects inA\n"
        "Code based upon: Ray Casting against General Convex Objects with Application to Continuous Collision Detection - Gino van den Bergen.\n"
        "Args:\n"
        "    ray_origin (Vec3): Origin of the ray.\n"
        "    ray_direction (Vec3): Direction of the ray (ioLambda * inDirection determines length).\n"
        "    tolerance (float): The minimal distance between the ray and A before it is considered colliding.\n"
        "    a (A): A convex object that has the GetSupport(Vec3) function.\n"
        "    lambda (float): The max fraction along the ray, on output updated with the actual collision fraction.\n"
        "Returns:\n"
        "    bool: true if a hit was found, ioLambda is the solution for lambda.")

    .def("cast_shape", [](GJKClosestPoint &self, Mat44Arg inStart, Vec3Arg inDirection,
        float inTolerance, const Sphere &inA, const Sphere &inB, float &ioLambda){

        auto r = self.CastShape(inStart,inDirection,inTolerance, inA, inB, ioLambda);
        return r;
    }, "start"_a, "direction"_a, "tolerance"_a, "a"_a, "b"_a, "lambda_"_a,
        "Test if a cast shape inA moving from inStart to lambda * inStart.GetTranslation() + inDirection where lambda e [0, ioLambda> intersects inB.\n"
        "Args:\n"
        "    start (Mat44): Start position and orientation of the convex object.\n"
        "    direction (Vec3): Direction of the sweep (ioLambda * inDirection determines length).\n"
        "    tolerance (float): The minimal distance between A and B before they are considered colliding.\n"
        "    a (A): The convex object A, must support the GetSupport(Vec3) function.\n"
        "    b (B): The convex object B, must support the GetSupport(Vec3) function.\n"
        "    lambda (float): The max fraction along the sweep, on output updated with the actual collision fraction.\n"
        "Returns:\n"
        "    bool: true if a hit was found, ioLambda is the solution for lambda.")

    .def("cast_shape", [](GJKClosestPoint &self, Mat44Arg inStart, Vec3Arg inDirection,
        float inTolerance, const Sphere &inA, const Sphere &inB, float inConvexRadiusA,
        float inConvexRadiusB, float &ioLambda){

        Vec3 outPointA, outPointB, outSeparatingAxis;
        auto r = self.CastShape(inStart, inDirection, inTolerance, inA, inB, ioLambda);
        return nb::make_tuple(r, outPointA, outPointB, outSeparatingAxis);
    }, "start"_a, "direction"_a, "tolerance"_a,
        "a"_a, "b"_a, "convex_radius_a"_a, "convex_radius_b"_a, "lambda_"_a,
        "Test if a cast shape inA moving from inStart to lambda * inStart.GetTranslation() + inDirection where lambda e [0, ioLambda> intersects inB.\n"
        "Args:\n"
        "    start (Mat44): Start position and orientation of the convex object.\n"
        "    direction (Vec3): Direction of the sweep (ioLambda * inDirection determines length).\n"
        "    tolerance (float): The minimal distance between A and B before they are considered colliding.\n"
        "    a (A): The convex object A, must support the GetSupport(Vec3) function.\n"
        "    b (B): The convex object B, must support the GetSupport(Vec3) function.\n"
        "    convex_radius_a (float): The convex radius of A, this will be added on all sides to pad A.\n"
        "    convex_radius_b (float): The convex radius of B, this will be added on all sides to pad B.\n"
        "    lambda (float): The max fraction along the sweep, on output updated with the actual collision fraction.\n"
        "    point_a (Vec3): is the contact point on A (if outSeparatingAxis is near zero, this may not be not the deepest point).\n"
        "    point_b (Vec3): is the contact point on B (if outSeparatingAxis is near zero, this may not be not the deepest point).\n"
        "    separating_axis (Vec3): On return this will contain a vector that points from A to B along the smallest distance of separation.\n"
        "The length of this vector indicates the separation of A and B without their convex radius.\n"
        "If it is near zero, the direction may not be accurate as the bodies may overlap when lambda = 0.\n"
        "Returns:\n"
        "    bool: true if a hit was found, ioLambda is the solution for lambda and outPoint and outSeparatingAxis are valid.");
}