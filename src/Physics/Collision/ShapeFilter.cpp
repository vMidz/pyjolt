#include "Common.h"
#include <Jolt/Physics/Collision/ShapeFilter.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <nanobind/trampoline.h>

class PyShapeFilter : public ShapeFilter {
  public:
    NB_TRAMPOLINE(ShapeFilter, 2);

    ~PyShapeFilter() override = default;

    bool ShouldCollide(const Shape *inShape2, const SubShapeID &inSubShapeIDOfShape2) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inShape2,
            inSubShapeIDOfShape2);
    }

    bool ShouldCollide(const Shape *inShape1, const SubShapeID &inSubShapeIDOfShape1, const Shape *inShape2, const SubShapeID &inSubShapeIDOfShape2) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inShape1,
            inSubShapeIDOfShape1,
            inShape2,
            inSubShapeIDOfShape2);
    }
};

void BindShapeFilter(nb::module_ &m) {
    nb::class_<ShapeFilter, PyShapeFilter> shapeFilterCls(m, "ShapeFilter",
        "Filter class");
    shapeFilterCls
        .def(nb::init<>())
        .def("should_collide", nb::overload_cast<const Shape *, const SubShapeID &>(&ShapeFilter::ShouldCollide, nb::const_), "shape2"_a, "sub_shape_id_of_shape2"_a,
            "Filter function to determine if we should collide with a shape. Returns true if the filter passes.\n"
            "This overload is called when the query doesn't have a source shape (e.g. ray cast / collide point).\n"
            "Args:\n"
            "    shape2 (Shape*): Shape we're colliding against.\n"
            "    sub_shape_id_of_shape2 (SubShapeID): The sub shape ID that will lead from the root shape to inShape2 (i.e. the shape of mBodyID2).")
        .def("should_collide", nb::overload_cast<const Shape *, const SubShapeID &, const Shape *, const SubShapeID &>(&ShapeFilter::ShouldCollide, nb::const_), "shape1"_a, "sub_shape_id_of_shape1"_a, "shape2"_a, "sub_shape_id_of_shape2"_a,
            "Filter function to determine if two shapes should collide. Returns true if the filter passes.\n"
            "This overload is called when querying a shape vs a shape (e.g. collide object / cast object).\n"
            "It is called at each level of the shape hierarchy, so if you have a compound shape with a box, this function will be called twice.\n"
            "It will not be called on triangles that are part of another shape, i.e a mesh shape will not trigger a callback per triangle. You can filter out individual triangles in the CollisionCollector::AddHit function by their sub shape ID.\n"
            "Args:\n"
            "    shape1 (Shape*): 1st shape that is colliding.\n"
            "    sub_shape_id_of_shape1 (SubShapeID): The sub shape ID that will lead from the root shape to inShape1 (i.e. the shape that is used to collide or cast against shape 2).\n"
            "    shape2 (Shape*): 2nd shape that is colliding.\n"
            "    sub_shape_id_of_shape2 (SubShapeID): The sub shape ID that will lead from the root shape to inShape2 (i.e. the shape of mBodyID2).")
        .def_rw("body_id2", &ShapeFilter::mBodyID2,
            "Set by the collision detection functions to the body ID of the body that we're colliding against before calling the ShouldCollide function");

    nb::class_<ReversedShapeFilter, ShapeFilter> reversedShapeFilterCls(m, "ReversedShapeFilter",
        "Helper class to reverse the order of the shapes in the ShouldCollide function");
    reversedShapeFilterCls
        .def(nb::init<const ShapeFilter &>(), "filter"_a,
            "Constructor")
        .def("should_collide", nb::overload_cast<const Shape *, const SubShapeID &>(&ReversedShapeFilter::ShouldCollide, nb::const_), "shape2"_a, "sub_shape_id_of_shape2"_a)
        .def("should_collide", nb::overload_cast<const Shape *, const SubShapeID &, const Shape *, const SubShapeID &>(&ReversedShapeFilter::ShouldCollide, nb::const_), "shape1"_a, "sub_shape_id_of_shape1"_a, "shape2"_a, "sub_shape_id_of_shape2"_a);
}