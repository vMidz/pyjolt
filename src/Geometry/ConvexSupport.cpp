#include "Common.h"
#include <Jolt/Geometry/ConvexSupport.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

void BindConvexSupport(nb::module_ &m) {
    using TransformedInst = TransformedConvexObject<AABox>;
    nb::class_<TransformedInst>(m, "TransformedConvexObject",
        "Helper functions to get the support point for a convex object\n"
        "Structure that transforms a convex object (supports only uniform scaling)")
        .def(nb::init<Mat44Arg, const AABox &>())
        .def("get_support", &TransformedInst::GetSupport, "direction"_a, "Calculate the support vector for this convex shape.")
        .def("get_supporting_face", [](TransformedInst &self, Vec3Arg inDirection) {
            Array<Vec3> out;
            self.GetSupportingFace(inDirection, out);
            return out;
        }, "Get the vertices of the face that faces inDirection the most")
        .def_rw("transform", &TransformedInst::mTransform)
        .def_prop_ro("object", [](TransformedInst &self) {
            return self.mObject;
        });

    using AddConvexRadiusInst = AddConvexRadius<AABox>;
    nb::class_<AddConvexRadiusInst>(m, "AddConvexRadius", "Structure that adds a convex radius")
        .def(nb::init<const AABox &, float>())
        .def("get_support", &AddConvexRadiusInst::GetSupport, "direction"_a, "Calculate the support vector for this convex shape.")
        .def_prop_ro("object", [](AddConvexRadiusInst &self) {
            return self.mObject;
        })
        .def_rw("radius", &AddConvexRadiusInst::mRadius);

    using MinkowskiDifferenceInst = MinkowskiDifference<AABox, AABox>;
    nb::class_<MinkowskiDifferenceInst>(m, "AddConvexRadius", "Structure that adds a convex radius")
        .def(nb::init<AABox, AABox>(), "object_a"_a, "object_b"_a)
        .def("get_support", &MinkowskiDifferenceInst::GetSupport, "direction"_a, "the support vector for this convex shape.")

        .def_prop_ro("object_a", [](MinkowskiDifferenceInst &self) {
            return self.mObjectA;
        })
        .def_prop_ro("object_b", [](MinkowskiDifferenceInst &self) {
            return self.mObjectB;
        });

    nb::class_<PointConvexSupport>(m, "PointConvexSupport", "Class that wraps a point so that it can be used with convex collision detection")
        .def("get_support", &PointConvexSupport::GetSupport, "direction"_a, "Calculate the support vector for this convex shape.")
        .def_rw("point", &PointConvexSupport::mPoint);

    nb::class_<TriangleConvexSupport>(m, "TriangleConvexSupport", "Class that wraps a triangle so that it can used with convex collision detection")
        .def(nb::init<Vec3Arg, Vec3Arg, Vec3Arg>(), "vector_1"_a, "vector_2"_a, "vector_3"_a, "Constructor")
        .def("get_support", &TriangleConvexSupport::GetSupport, "direction"_a, "Calculate the support vector for this convex shape.")

        .def("get_supporting_face", [](TriangleConvexSupport &self, Vec3Arg direction) {
            Array<Vec3> vertexOut;
            self.GetSupportingFace(direction, vertexOut);
            return vertexOut;
        }, "direction"_a, "Get the vertices of the face that faces inDirection the most")

        .def_rw("v1", &TriangleConvexSupport::mV1, "The three vertices of the triangle")
        .def_rw("v2", &TriangleConvexSupport::mV2)
        .def_rw("v3", &TriangleConvexSupport::mV3);

    using Inst = PolygonConvexSupport<Array<Vec3>>;
    nb::class_<Inst>(m, "PolygonConvexSupport", "Class that wraps a polygon so that it can used with convex collision detection")
        .def(nb::init<const Array<Vec3> &>(), "Constructor")
        .def("get_support", &Inst::GetSupport, "direction"_a, "Calculate the support vector for this convex shape.")
        .def("get_supporting_face", [](Inst &self, [[maybe_unused]] Vec3Arg inDirection) {
            Array<Vec3> outVertices;
            self.GetSupportingFace(inDirection, outVertices);
            return outVertices;
        }, "direction"_a, "Get the vertices of the face that faces inDirection the most")

        .def_prop_ro("vertices", [](Inst &self) {
            return self.mVertices;
        });
}