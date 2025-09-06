#include "Common.h"
#include <Jolt/Physics/Collision/CollisionDispatch.h>

void BindCollisionDispatch(nb::module_ &m) {
    nb::class_<CollisionDispatch> collisionDispatchCls(m, "CollisionDispatch",
        "Dispatch function, main function to handle collisions between shapes");
    collisionDispatchCls
        .def_static("collide_shape_vs_shape", &CollisionDispatch::sCollideShapeVsShape, "shape1"_a, "shape2"_a, "scale1"_a, "scale2"_a, "center_of_mass_transform1"_a, "center_of_mass_transform2"_a, "sub_shape_id_creator1"_a, "sub_shape_id_creator2"_a, "collide_shape_settings"_a, "collector"_a, "shape_filter"_a,
            "Collide 2 shapes and pass any collision on to ioCollector.\n"
            "Args:\n"
            "    shape1 (Shape*): The first shape.\n"
            "    shape2 (Shape*): The second shape.\n"
            "    scale1 (Vec3): Local space scale of shape 1 (scales relative to its center of mass).\n"
            "    scale2 (Vec3): Local space scale of shape 2 (scales relative to its center of mass).\n"
            "    center_of_mass_transform1 (Mat44): Transform to transform center of mass of shape 1 into world space.\n"
            "    center_of_mass_transform2 (Mat44): Transform to transform center of mass of shape 2 into world space.\n"
            "    sub_shape_id_creator1 (SubShapeIDCreator): Class that tracks the current sub shape ID for shape 1.\n"
            "    sub_shape_id_creator2 (SubShapeIDCreator): Class that tracks the current sub shape ID for shape 2.\n"
            "    collide_shape_settings (CollideShapeSettings): Options for the CollideShape test.\n"
            "    collector (CollideShapeCollector): The collector that receives the results.\n"
            "    shape_filter (ShapeFilter): allows selectively disabling collisions between pairs of (sub) shapes.")
        .def_static("cast_shape_vs_shape_local_space", &CollisionDispatch::sCastShapeVsShapeLocalSpace, "shape_cast_local"_a, "shape_cast_settings"_a, "shape"_a, "scale"_a, "shape_filter"_a, "center_of_mass_transform2"_a, "sub_shape_id_creator1"_a, "sub_shape_id_creator2"_a, "collector"_a,
            "Cast a shape against this shape, passes any hits found to ioCollector.\n"
            "Note: This version takes the shape cast in local space relative to the center of mass of inShape, take a look at sCastShapeVsShapeWorldSpace if you have a shape cast in world space.\n"
            "Args:\n"
            "    shape_cast_local (ShapeCast): The shape to cast against the other shape and its start and direction.\n"
            "    shape_cast_settings (ShapeCastSettings): Settings for performing the cast.\n"
            "    shape (Shape*): The shape to cast against.\n"
            "    scale (Vec3): Local space scale for the shape to cast against (scales relative to its center of mass).\n"
            "    shape_filter (ShapeFilter): allows selectively disabling collisions between pairs of (sub) shapes.\n"
            "    center_of_mass_transform2 (Mat44): Is the center of mass transform of shape 2 (excluding scale), this is used to provide a transform to the shape cast result so that local hit result quantities can be transformed into world space.\n"
            "    sub_shape_id_creator1 (SubShapeIDCreator): Class that tracks the current sub shape ID for the casting shape.\n"
            "    sub_shape_id_creator2 (SubShapeIDCreator): Class that tracks the current sub shape ID for the shape we're casting against.\n"
            "    collector (CastShapeCollector): The collector that receives the results.")
        .def_static("cast_shape_vs_shape_world_space", &CollisionDispatch::sCastShapeVsShapeWorldSpace, "shape_cast_world"_a, "shape_cast_settings"_a, "shape"_a, "scale"_a, "shape_filter"_a, "center_of_mass_transform2"_a, "sub_shape_id_creator1"_a, "sub_shape_id_creator2"_a, "collector"_a,
            "See: sCastShapeVsShapeLocalSpace.\n"
            "The only difference is that the shape cast (inShapeCastWorld) is provided in world space.\n"
            "Note: A shape cast contains the center of mass start of the shape, if you have the world transform of the shape you probably want to construct it using ShapeCast::sFromWorldTransform.")
        .def_static("init", &CollisionDispatch::sInit,
            "Initialize all collision functions with a function that asserts and returns no collision")
        // .def_static("register_collide_shape", &CollisionDispatch::sRegisterCollideShape, "type1"_a, "type2"_a, "function"_a, // TODO:
            // "Register a collide shape function in the collision table")
        // .def_static("register_cast_shape", &CollisionDispatch::sRegisterCastShape, "type1"_a, "type2"_a, "function"_a,       // TODO:
            // "Register a cast shape function in the collision table")
        .def_static("reversed_collide_shape", &CollisionDispatch::sReversedCollideShape, "shape1"_a, "shape2"_a, "scale1"_a, "scale2"_a, "center_of_mass_transform1"_a, "center_of_mass_transform2"_a, "sub_shape_id_creator1"_a, "sub_shape_id_creator2"_a, "collide_shape_settings"_a, "collector"_a, "shape_filter"_a,
            "An implementation of CollideShape that swaps inShape1 and inShape2 and swaps the result back, can be registered if the collision function only exists the other way around")
        .def_static("reversed_cast_shape", &CollisionDispatch::sReversedCastShape, "shape_cast"_a, "shape_cast_settings"_a, "shape"_a, "scale"_a, "shape_filter"_a, "center_of_mass_transform2"_a, "sub_shape_id_creator1"_a, "sub_shape_id_creator2"_a, "collector"_a,
            "An implementation of CastShape that swaps inShape1 and inShape2 and swaps the result back, can be registered if the collision function only exists the other way around");
}