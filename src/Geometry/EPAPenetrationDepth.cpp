#include "Common.h"
#include <Jolt/Geometry/EPAPenetrationDepth.h>
#include <Jolt/Geometry/ConvexSupport.h>
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>

void BindEPAPenetrationDepth(nb::module_ &m) {
    auto doc = "Implementation of Expanding Polytope Algorithm as described in:\n"
               "Proximity Queries and Penetration Depth Computation on 3D Game Objects - Gino van den Bergen\n"
               "The implementation of this algorithm does not completely follow the article, instead of splitting\n"
               "triangles at each edge as in fig. 7 in the article, we build a convex hull (removing any triangles that\n"
               "are facing the new point, thereby avoiding the problem of getting really oblong triangles as mentioned in\n"
               "the article).\n"
               "The algorithm roughly works like:\n"
               "- Start with a simplex of the Minkowski sum (difference) of two objects that was calculated by GJK\n"
               "- This simplex should contain the origin (or else GJK would have reported: no collision)\n"
               "- In cases where the simplex consists of 1 - 3 points, find some extra support points (of the Minkowski sum) to get to at least 4 points\n"
               "- Convert this into a convex hull with non-zero volume (which includes the origin)\n"
               "- A: Calculate the closest point to the origin for all triangles of the hull and take the closest one\n"
               "- Calculate a new support point (of the Minkowski sum) in this direction and add this point to the convex hull\n"
               "- This will remove all faces that are facing the new point and will create new triangles to fill up the hole\n"
               "- Loop to A until no closer point found\n"
               "- The closest point indicates the position / direction of least penetration";

    nb::class_<EPAPenetrationDepth> epaPenetrationDepthCls(m, "EPAPenetrationDepth", doc);
    epaPenetrationDepthCls
        .def("get_penetration_depth_step_gjk", [](EPAPenetrationDepth &self, const ConvexShape::Support &inAExcludingConvexRadius,
            float inConvexRadiusA, const TriangleConvexSupport &inBExcludingConvexRadius, float inConvexRadiusB, float inTolerance, Vec3 ioV) {
            Vec3 outPointA;
            Vec3 outPointB;
            auto r = self.GetPenetrationDepthStepGJK(inAExcludingConvexRadius, inConvexRadiusA,
                inBExcludingConvexRadius, inConvexRadiusB, inTolerance, ioV, outPointA, outPointB);
            return nb::make_tuple(r, outPointA, outPointB);
        },  "Calculates penetration depth between two objects, first step of two (the GJK step).\n"
            "Args:\n"
            "    a_excluding_convex_radius (AE): Object A without convex radius.\n"
            "    b_excluding_convex_radius (BE): Object B without convex radius.\n"
            "    convex_radius_a (float): Convex radius for A.\n"
            "    convex_radius_b (float): Convex radius for B.\n"
            "    v (Vec3): Pass in previously returned value or (1, 0, 0). On return this value is changed to direction to move B out of collision along the shortest path (magnitude is meaningless).\n"
            "    tolerance (float): Minimal distance before A and B are considered colliding.\n"
            "    point_a (Vec3): Position on A that has the least amount of penetration.\n"
            "    point_b (Vec3): Position on B that has the least amount of penetration.\n"
            "Use |outPointB - outPointA| to get the distance of penetration.\n"
            "Output: [status, outPointA, outPointB]")

        .def("get_penetration_depth_step_epa", [](EPAPenetrationDepth &self, AddConvexRadius<ConvexShape::Support> inAIncludingConvexRadius,
            const TriangleConvexSupport &inBIncludingConvexRadius, float inTolerance){
            Vec3 outV, outPointA, outPointB;
            auto r = self.GetPenetrationDepthStepEPA(inAIncludingConvexRadius, inBIncludingConvexRadius, inTolerance, outV, outPointA, outPointB);
            return nb::make_tuple(r, outV ,outPointA, outPointB);
        },  "Calculates penetration depth between two objects, second step (the EPA step).\n"
            "Args:\n"
            "    a_including_convex_radius (AI): Object A with convex radius.\n"
            "    b_including_convex_radius (BI): Object B with convex radius.\n"
            "    tolerance (float): A factor that determines the accuracy of the result. If the change of the squared distance is less than inTolerance * current_penetration_depth^2 the algorithm will terminate. Should be bigger or equal to FLT_EPSILON.\n"
            "    v (Vec3): Direction to move B out of collision along the shortest path (magnitude is meaningless).\n"
            "    point_a (Vec3): Position on A that has the least amount of penetration.\n"
            "    point_b (Vec3): Position on B that has the least amount of penetration\n"
            "Use |outPointB - outPointA| to get the distance of penetration.\n"
            "Returns:\n"
            "    bool: False if the objects don't collide, in this case outPointA/outPointB are invalid.\n"
            "True if the objects penetrate.\n"
            "Output [status, outV ,outPointA, outPointB]")

        .def("get_penetration_depth", [](EPAPenetrationDepth &self,
            const ConvexShape::Support &inAExcludingConvexRadius, const AddConvexRadius<ConvexShape::Support> &inAIncludingConvexRadius,
            float inConvexRadiusA, const TriangleConvexSupport &inBExcludingConvexRadius, const TriangleConvexSupport &inBIncludingConvexRadius, float inConvexRadiusB,
            float inCollisionToleranceSq, float inPenetrationTolerance, Vec3 &ioV){

            Vec3 outPointA, outPointB;
            auto r = self.GetPenetrationDepth(inAExcludingConvexRadius, inAIncludingConvexRadius, inConvexRadiusA, inBExcludingConvexRadius, inBIncludingConvexRadius,
                inConvexRadiusB, inCollisionToleranceSq, inPenetrationTolerance, ioV, outPointA, outPointB);
            return nb::make_tuple(r, outPointA, outPointB);
        }, "This function combines the GJK and EPA steps and is provided as a convenience function.\n"
           "Note: less performant since you're providing all support functions in one go\n"
           "Note 2: You need to initialize ioV, see documentation at GetPenetrationDepthStepGJK!\n"
           "Output [r, outPointA, outPointB]")
        // TODO:
            // .def("cast_shape", &EPAPenetrationDepth::CastShape, "start"_a, "direction"_a, "collision_tolerance"_a, "penetration_tolerance"_a, "a"_a, "b"_a, "convex_radius_a"_a, "convex_radius_b"_a, "return_deepest_point"_a, "lambda_"_a, "point_a"_a, "point_b"_a, "contact_normal"_a,
            // "Test if a cast shape inA moving from inStart to lambda * inStart.GetTranslation() + inDirection where lambda e [0, ioLambda> intersects inB.\n"
            // "Args:\n"
            // "    start (Mat44): Start position and orientation of the convex object.\n"
            // "    direction (Vec3): Direction of the sweep (ioLambda * inDirection determines length).\n"
            // "    collision_tolerance (float): The minimal distance between A and B before they are considered colliding.\n"
            // "    penetration_tolerance (float): A factor that determines the accuracy of the result. If the change of the squared distance is less than inTolerance * current_penetration_depth^2 the algorithm will terminate. Should be bigger or equal to FLT_EPSILON.\n"
            // "    a (A): The convex object A, must support the GetSupport(Vec3) function.\n"
            // "    b (B): The convex object B, must support the GetSupport(Vec3) function.\n"
            // "    convex_radius_a (float): The convex radius of A, this will be added on all sides to pad A.\n"
            // "    convex_radius_b (float): The convex radius of B, this will be added on all sides to pad B.\n"
            // "    return_deepest_point (bool): If the shapes are initially intersecting this determines if the EPA algorithm will run to find the deepest point.\n"
            // "    lambda (float): The max fraction along the sweep, on output updated with the actual collision fraction.\n"
            // "    point_a (Vec3): is the contact point on A.\n"
            // "    point_b (Vec3): is the contact point on B.\n"
            // "    contact_normal (Vec3): is either the contact normal when the objects are touching or the penetration axis when the objects are penetrating at the start of the sweep (pointing from A to B, length will not be 1).\n"
            // "Returns:\n"
            // "    bool: true if the a hit was found, in which case ioLambda, outPointA, outPointB and outSurfaceNormal are updated.");
        ;

    nb::enum_<EPAPenetrationDepth::EStatus>(epaPenetrationDepthCls, "EStatus", "Return code for GetPenetrationDepthStepGJK")
        .value("NotColliding", EPAPenetrationDepth::EStatus::NotColliding, "Returned if the objects don't collide, in this case outPointA/outPointB are invalid")
        .value("Colliding", EPAPenetrationDepth::EStatus::Colliding, "Returned if the objects penetrate")
        .value("Indeterminate", EPAPenetrationDepth::EStatus::Indeterminate, "Returned if the objects penetrate further than the convex radius. In this case you need to call GetPenetrationDepthStepEPA to get the actual penetration depth.");
}