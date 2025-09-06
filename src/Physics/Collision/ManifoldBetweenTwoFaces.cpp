#include "Common.h"
#include <Jolt/Physics/Collision/ManifoldBetweenTwoFaces.h>

void BindManifoldBetweenTwoFaces(nb::module_ &m) {
    m.def("prune_contact_points", &PruneContactPoints, "penetration_axis"_a, "contact_points_on1"_a, "contact_points_on2"_a, "center_of_mass"_a,
        "Remove contact points if there are > 4 (no more than 4 are needed for a stable solution).\n"
        "Args:\n"
        "    penetration_axis (Vec3): is the world space penetration axis (must be normalized).\n"
        "    contact_points_on1 (ContactPoints): The contact points on shape 1 relative to inCenterOfMass.\n"
        "    contact_points_on2 (ContactPoints): The contact points on shape 2 relative to inCenterOfMass\n"
        "On output ioContactPointsOn1/2 are reduced to 4 or less points.\n"
        "    center_of_mass (Vec3): Center of mass position of body 1.");

    m.def("manifold_between_two_faces", &ManifoldBetweenTwoFaces, "contact_point1"_a, "contact_point2"_a, "penetration_axis"_a, "max_contact_distance_sq"_a, "shape1_face"_a, "shape2_face"_a, "contact_points1"_a, "contact_points2"_a, "center_of_mass"_a,
        "Determine contact points between 2 faces of 2 shapes and return them in outContactPoints 1 & 2.\n"
        "Args:\n"
        "    contact_point1 (Vec3): The contact point on shape 1 relative to inCenterOfMass.\n"
        "    contact_point2 (Vec3): The contact point on shape 2 relative to inCenterOfMass.\n"
        "    penetration_axis (Vec3): The local space penetration axis in world space.\n"
        "    max_contact_distance_sq (float): After face 2 is clipped against face 1, each remaining point on face 2 is tested against the plane of face 1. If the distance^2 on the positive side of the plane is larger than this distance, the point will be discarded as a contact point.\n"
        "    shape1_face (SupportingFace): The supporting faces on shape 1 relative to inCenterOfMass.\n"
        "    shape2_face (SupportingFace): The supporting faces on shape 2 relative to inCenterOfMass.\n"
        "    contact_points1 (ContactPoints): Returns the contact points between the two shapes for shape 1 relative to inCenterOfMass (any existing points in the output array are left as is).\n"
        "    contact_points2 (ContactPoints): Returns the contact points between the two shapes for shape 2 relative to inCenterOfMass (any existing points in the output array are left as is).\n"
        "    center_of_mass (Vec3): Center of mass position of body 1.");
}