from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class CenterOfMassTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        # Compound shape with center of mass offset
        compound_shape1 = pyjolt.StaticCompoundShapeSettings()
        compound_shape1.add_shape(Vec3(10, 0, 0), Quat.identity(), pyjolt.SphereShape(2))
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            compound_shape1, Vec3(0, 10.0, 0), Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, Layers.MOVING), pyjolt.EActivation.ACTIVATE)

        # Create box with center of mass offset
        box_points = pyjolt.Vec3Array()
        box_points.push_back(Vec3(10, 10, 10))
        box_points.push_back(Vec3(5, 10, 10))
        box_points.push_back(Vec3(10, 5, 10))
        box_points.push_back(Vec3(5, 5, 10))
        box_points.push_back(Vec3(10, 10, 5))
        box_points.push_back(Vec3(5, 10, 5))
        box_points.push_back(Vec3(10, 5, 5))
        box_points.push_back(Vec3(5, 5, 5))

        convex_hull_settings = pyjolt.ConvexHullShapeSettings(box_points)
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            convex_hull_settings, Vec3(0, 10.0, 20.0), 
            Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI), 
            pyjolt.EMotionType.DYNAMIC, Layers.MOVING), pyjolt.EActivation.ACTIVATE)

        # Compound
        compound_shape2 = pyjolt.StaticCompoundShapeSettings()
        rotation = Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI)
        compound_shape2.add_shape(Vec3(10, 0, 0), rotation, pyjolt.CapsuleShape(5, 1))
        compound_shape2.add_shape(rotation * Vec3(10, -5, 0), Quat.identity(), pyjolt.SphereShape(4))
        compound_shape2.add_shape(rotation * Vec3(10, 5, 0), Quat.identity(), pyjolt.SphereShape(2))
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            compound_shape2, Vec3(0, 10.0, 40.0), Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, Layers.MOVING), pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()