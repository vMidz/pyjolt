from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat
import random

class FunnelTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        box = pyjolt.BoxShape(Vec3(50.0, 1.0, 50.0), 0.0)

        # Funnel
        for i in range(4):
            rotation = Quat.rotation(Vec3.axis_y(), 0.5 * math.JPH_PI * i)
            position = rotation * Vec3(25.0, 25.0, 0.0)
            shape_rotation = rotation * Quat.rotation(Vec3.axis_z(), 0.25 * math.JPH_PI)
            
            settings = pyjolt.BodyCreationSettings(
                box, position, shape_rotation, 
                pyjolt.EMotionType.STATIC, Layers.NON_MOVING
            )
            body_interface.create_and_add_body(settings, pyjolt.EActivation.DONT_ACTIVATE)

        shape = None
        for obj_it in range(1000):
            # Random scale
            scale_components = [random.uniform(-1.5, 1.5) for _ in range(3)]
            
            # Make it minimally -0.5 or 0.5 depending on the sign
            final_components = [s + (0.5 if s >= 0 else -0.5) for s in scale_components]
            scale = Vec3(final_components[0], final_components[1], final_components[2])

            shape_type = random.randint(0, 9)

            if shape_type == 0: # Sphere
                shape = pyjolt.SphereShape(random.uniform(0.1, 2.0))
                scale = Vec3(scale.x, scale.x, scale.x) # Only uniform scale supported
            
            elif shape_type == 1: # Box
                f_size = random.uniform(0.1, 2.0)
                shape = pyjolt.BoxShape(Vec3(f_size, f_size, f_size))

            elif shape_type == 2: # Convex Hull
                points = pyjolt.Vec3Array()
                for _ in range(20):
                    f_size = random.uniform(0.1, 2.0)
                    points.push_back(f_size * Vec3.random())
                shape = pyjolt.ConvexHullShapeSettings(points).create().get()

            elif shape_type == 3: # Capsule
                f_size = random.uniform(0.1, 2.0)
                shape = pyjolt.CapsuleShape(0.5 * f_size, f_size)
                scale = Vec3(scale.x, scale.x, scale.x) # Only uniform scale supported
            
            elif shape_type == 4: # Tapered Capsule
                top = random.uniform(0.1, 2.0)
                bottom = random.uniform(0.1, 2.0)
                half_height = max(0.5 * random.uniform(0.1, 2.0), 0.5 * abs(top - bottom) + 0.001)
                shape = pyjolt.TaperedCapsuleShapeSettings(half_height, top, bottom).create().get()
                scale = Vec3(scale.x, scale.x, scale.x) # Only uniform scale supported
            
            elif shape_type == 5: # Cylinder
                f_size = random.uniform(0.1, 2.0)
                shape = pyjolt.CylinderShape(0.5 * f_size, f_size)
                scale = Vec3(scale.x, scale.y, scale.x) # Scale X must be same as Z
            
            elif shape_type == 6: # Tapered Cylinder
                f_size1 = random.uniform(0.1, 2.0)
                f_size2 = random.uniform(0.1, 2.0)
                shape = pyjolt.TaperedCylinderShapeSettings(0.5 * f_size1, f_size2, f_size2).create().get()
                scale = Vec3(scale.x, scale.y, scale.x) # Scale X must be same as Z
            
            elif shape_type == 7: # Cone (Tapered Cylinder with 0 radius)
                f_size1 = random.uniform(0.1, 2.0)
                shape = pyjolt.TaperedCylinderShapeSettings(0.5 * f_size1, 0.0, f_size1, 0.0).create().get()
                scale = Vec3(scale.x, scale.y, scale.x) # Scale X must be same as Z

            elif shape_type == 8: # Simple Compound
                compound_settings = pyjolt.StaticCompoundShapeSettings()
                compound_settings.add_shape(Vec3.zero(), Quat.identity(), pyjolt.CapsuleShape(1.0, 0.1))
                compound_settings.add_shape(Vec3(0, -1, 0), Quat.identity(), pyjolt.SphereShape(0.5))
                compound_settings.add_shape(Vec3(0, 1, 0), Quat.identity(), pyjolt.SphereShape(0.5))
                shape = compound_settings.create().get()
                scale = Vec3(scale.x, scale.x, scale.x) # Only uniform scale supported
            
            elif shape_type == 9: # Compound with sub compound and rotation
                sub_compound_settings = pyjolt.StaticCompoundShapeSettings()
                sub_compound_settings.add_shape(Vec3(0, 0.75, 0), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), pyjolt.BoxShape(Vec3(0.75, 0.25, 0.2)))
                sub_compound_settings.add_shape(Vec3(0.75, 0, 0), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), pyjolt.CylinderShape(0.75, 0.2))
                sub_compound_settings.add_shape(Vec3(0, 0, 0.75), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), pyjolt.TaperedCapsuleShapeSettings(0.75, 0.25, 0.2))

                compound_settings = pyjolt.StaticCompoundShapeSettings()
                rot1 = Quat.rotation(Vec3.axis_x(), -0.25 * math.JPH_PI) * Quat.rotation(Vec3.axis_z(), 0.25 * math.JPH_PI)
                rot2 = Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI) * Quat.rotation(Vec3.axis_z(), -0.75 * math.JPH_PI)
                compound_settings.add_shape(Vec3(0, 0, 0), rot1, sub_compound_settings)
                compound_settings.add_shape(Vec3(0, -0.1, 0), rot2, sub_compound_settings)
                shape = compound_settings.create().get()
                scale = Vec3(scale.x, scale.x, scale.x) # Only uniform scale supported

            # Scale the shape
            if random.randint(0, 2) == 0:
                shape = pyjolt.ScaledShape(shape, scale)

            position = Vec3(random.uniform(-40, 40), 100.0 + random.uniform(-40, 40), random.uniform(-40, 40))
            body_settings = pyjolt.BodyCreationSettings(
                shape, position, Quat.random(), 
                pyjolt.EMotionType.DYNAMIC, Layers.MOVING
            )

            body_interface.create_and_add_body(body_settings, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()