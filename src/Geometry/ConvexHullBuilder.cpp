#include "Common.h"
#include <Jolt/Geometry/ConvexHullBuilder.h>

void BindConvexHullBuilder(nb::module_ &m) {
    nb::class_<ConvexHullBuilder, NonCopyable>convexHullBuilderCls(m, "ConvexHullBuilder",
        "A convex hull builder that tries to create hulls as accurately as possible. Used for offline processing.");
    convexHullBuilderCls
        .def("initialize", [](ConvexHullBuilder &self, int inMaxVertices, float inTolerance) {
            const char *out;
            self.Initialize(inMaxVertices, inTolerance, out);
            return nb::str(out);
        }, "max_vertices"_a, "tolerance"_a,
            "Takes all positions as provided by the constructor and use them to build a hull\n"
            "Any points that are closer to the hull than inTolerance will be discarded.\n"
            "Args:\n"
            "    max_vertices (int): Max vertices to allow in the hull. Specify INT_MAX if there is no limit.\n"
            "    tolerance (float): Max distance that a point is allowed to be outside of the hull.\n"
            "    error (str): Error message when building fails.\n"
            "Returns:\n"
            "    EResult: Status code that reports if the hull was created or not.")

        .def("get_num_vertices_used", &ConvexHullBuilder::GetNumVerticesUsed, "Returns the amount of vertices that are currently used by the hull")
        .def("contains_face", [](ConvexHullBuilder &self, const ConvexHullBuilder::ConflictList inIndices) {
            bool status = self.ContainsFace(inIndices);
            return status;
        }, "indices"_a, "Returns true if the hull contains a polygon with inIndices (counter clockwise indices in mPositions)")

        .def("get_center_of_mass_and_volume", [](ConvexHullBuilder &self) {
            float outVolume;
            Vec3 outCenterOfMass;
            self.GetCenterOfMassAndVolume(outCenterOfMass, outVolume);
            return nb::make_tuple(outVolume, outCenterOfMass);
        }, "Calculate the center of mass and the volume of the current convex hull\n"
           "Output: [outVolume, outCenterOfMass]")

        .def("determine_max_error", [](ConvexHullBuilder &self) {
            ConvexHullBuilder::Face *outFaceWithMaxError;
            float outMaxError;
            int outMaxErrorPositionIdx;
            float outCoplanarDistance;
            self.DetermineMaxError(outFaceWithMaxError, outMaxError, outMaxErrorPositionIdx, outCoplanarDistance);
            nb::make_tuple(outMaxError, outMaxErrorPositionIdx, outCoplanarDistance);
        }, "Determines the point that is furthest outside of the hull and reports how far it is outside of the hull (which indicates a failure during hull building).\n"
           "Args:\n"
           "    face_with_max_error (Face*): The face that caused the error.\n"
           "    max_error (float): The maximum distance of a point to the hull.\n"
           "    max_error_position_idx (int): The index of the point that had this distance.\n"
           "    coplanar_distance (float): Points that are less than this distance from the hull are considered on the hull. This should be used as a lowerbound for the allowed error.\n"
           "Output: [outMaxError,outMaxErrorPositionIdx,outCoplanarDistance]")
        .def("get_faces", &ConvexHullBuilder::GetFaces, "Access to the created faces. Memory is owned by the convex hull builder.");

    nb::class_<ConvexHullBuilder::Edge, NonCopyable>(convexHullBuilderCls, "Edge", "Class that holds the information of an edge")
        .def(nb::init<ConvexHullBuilder::Face*, int>(), "face"_a, "start_index"_a)
        .def("get_previous_edge", &ConvexHullBuilder::Edge::GetPreviousEdge)
        .def_rw("face", &ConvexHullBuilder::Edge::mFace,"Face that this edge belongs to")
        .def_rw("next_edge", &ConvexHullBuilder::Edge::mNextEdge,"Next edge of this face")
        .def_rw("neighbour_edge", &ConvexHullBuilder::Edge::mNeighbourEdge, "Edge that this edge is connected to")
        .def_rw("start_idx", &ConvexHullBuilder::Edge::mStartIdx, "Vertex index in mPositions that indicates the start vertex of this edge");

    nb::class_<ConvexHullBuilder::Face, NonCopyable>(convexHullBuilderCls, "Face", "Class that holds the information of one face")
        .def("initialize", &ConvexHullBuilder::Face::Initialize,
            "index_0"_a, "index_1"_a, "index_2"_a, "positions"_a, "Initialize a face with three indices")
        .def("calculate_normal_and_centroid", &ConvexHullBuilder::Face::CalculateNormalAndCentroid,
            "positions"_a, "Calculates the centroid and normal for this face")
        .def("is_facing", &ConvexHullBuilder::Face::IsFacing, "positions"_a, "Check if face inFace is facing inPosition")
        .def_rw("mNormal", &ConvexHullBuilder::Face::mNormal, "Normal of this face, length is 2 times area of face")
        .def_rw("mCentroid", &ConvexHullBuilder::Face::mCentroid, "Center of the face")
        .def_rw("mConflictList", &ConvexHullBuilder::Face::mConflictList,
            "Positions associated with this edge (that are closest to this edge). The last position in the list is the point that is furthest away from the face")
        .def_rw("mFurthestPointDistanceSq", &ConvexHullBuilder::Face::mFurthestPointDistanceSq,
            "Squared distance of furthest point from the conflict list to the face")
        .def_rw("mRemoved", &ConvexHullBuilder::Face::mRemoved, "Flag that indicates that face has been removed (face will be freed later)");

    nb::enum_<ConvexHullBuilder::EResult>(convexHullBuilderCls, "EResult",
        "A convex hull builder that tries to create hulls as accurately as possible. Used for offline processing.")
        .value("SUCCESS", ConvexHullBuilder::EResult::Success, "Hull building finished successfully")
        .value("MAX_VERTICES_REACHED", ConvexHullBuilder::EResult::MaxVerticesReached,
            "Hull building finished successfully, but the desired accuracy was not reached because the max vertices limit was reached")
        .value("TOO_FEW_POINTS", ConvexHullBuilder::EResult::TooFewPoints, "Too few points to create a hull")
        .value("TOO_FEW_FACES", ConvexHullBuilder::EResult::TooFewFaces, "Too few faces in the created hull (signifies precision errors during building)")
        .value("DEGENERATE", ConvexHullBuilder::EResult::Degenerate, "Degenerate hull detected");
}