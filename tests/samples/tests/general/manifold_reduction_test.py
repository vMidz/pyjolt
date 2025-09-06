from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math, Triangle
from pyjolt.math import Vec3, Quat, Float3

class ManifoldReductionTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        c_perturbance = 0.02

        # Create mesh of regular grid of triangles
        triangles = pyjolt.TriangleArray()
        for x in range(-10, 10):
            for z in range(-10, 10):
                x1 = 0.1 * x
                z1 = 0.1 * z
                x2 = x1 + 0.1
                z2 = z1 + 0.1

                # Assuming perlin_noise3 is exposed in the bindings
                v1 = Float3(x1, c_perturbance * pyjolt.perlin_noise3(x1, 0, z1, 256, 256, 256), z1)
                v2 = Float3(x2, c_perturbance * pyjolt.perlin_noise3(x2, 0, z1, 256, 256, 256), z1)
                v3 = Float3(x1, c_perturbance * pyjolt.perlin_noise3(x1, 0, z2, 256, 256, 256), z2)
                v4 = Float3(x2, c_perturbance * pyjolt.perlin_noise3(x2, 0, z2, 256, 256, 256), z2)

                triangles.push_back(Triangle(v1, v3, v4, 0))
                triangles.push_back(Triangle(v1, v4, v2, 0))

        materials = pyjolt.PhysicsMaterialListArray()
        materials.push_back(pyjolt.PhysicsMaterialRefConst())
        mesh_shape = pyjolt.MeshShapeSettings(triangles, materials)

        # Floor
        scaled_floor_shape = pyjolt.ScaledShapeSettings(mesh_shape, Vec3.replicate(20))
        floor_settings = pyjolt.BodyCreationSettings(
            scaled_floor_shape, Vec3.zero(), Quat.identity(),
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING)
        body_interface.create_and_add_body(floor_settings, pyjolt.EActivation.DONT_ACTIVATE)

        # Create a box made of meshes
        mesh_box_shape = pyjolt.StaticCompoundShapeSettings()
        mesh_box_shape.add_shape(Vec3(0, -1, 0), Quat.rotation(Vec3.axis_x(), math.JPH_PI), mesh_shape)
        mesh_box_shape.add_shape(Vec3(0, 1, 0), Quat.identity(), mesh_shape)
        mesh_box_shape.add_shape(Vec3(-1, 0, 0), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), mesh_shape)
        mesh_box_shape.add_shape(Vec3(1, 0, 0), Quat.rotation(Vec3.axis_z(), -0.5 * math.JPH_PI), mesh_shape)
        mesh_box_shape.add_shape(Vec3(0, 0, -1), Quat.rotation(Vec3.axis_x(), -0.5 * math.JPH_PI), mesh_shape)
        mesh_box_shape.add_shape(Vec3(0, 0, 1), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), mesh_shape)

        # A convex box
        box_shape = pyjolt.BoxShapeSettings(Vec3(1, 1, 1), 0.0)

        three_mesh_box_shape = pyjolt.StaticCompoundShapeSettings()
        three_mesh_box_shape.add_shape(Vec3(-2.1, 0, 0), Quat.identity(), mesh_box_shape)
        three_mesh_box_shape.add_shape(Vec3(0, -1, 0), Quat.identity(), mesh_box_shape)
        three_mesh_box_shape.add_shape(Vec3(2.1, 0, 0), Quat.identity(), mesh_box_shape)

        three_box_shape = pyjolt.StaticCompoundShapeSettings()
        three_box_shape.add_shape(Vec3(-2.1, 0, 0), Quat.identity(), box_shape)
        three_box_shape.add_shape(Vec3(0, -1.1, 0), Quat.identity(), box_shape)
        three_box_shape.add_shape(Vec3(2.1, 0, 0), Quat.identity(), box_shape)

        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(three_mesh_box_shape, Vec3(0, 1, 0), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)

        box_settings = pyjolt.BodyCreationSettings(three_box_shape, Vec3(0, 2.95, 0), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        box_settings.allow_sleeping = False
        body_interface.create_and_add_body(box_settings, pyjolt.EActivation.ACTIVATE)

        two_mesh_box_shape = pyjolt.StaticCompoundShapeSettings()
        two_mesh_box_shape.add_shape(Vec3(-2.1, 0, 0), Quat.identity(), mesh_box_shape)
        two_mesh_box_shape.add_shape(Vec3(0, -1, 0), Quat.identity(), mesh_box_shape)

        two_box_shape = pyjolt.StaticCompoundShapeSettings()
        two_box_shape.add_shape(Vec3(-2.1, 0, 0), Quat.identity(), box_shape)
        two_box_shape.add_shape(Vec3(0, -1, 0), Quat.identity(), box_shape)

        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(two_mesh_box_shape, Vec3(0, 1, 4), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)

        box_settings = pyjolt.BodyCreationSettings(two_box_shape, Vec3(0, 4, 4), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        box_settings.allow_sleeping = False
        body_interface.create_and_add_body(box_settings, pyjolt.EActivation.ACTIVATE)

        two_mesh_shape_small_angle = pyjolt.StaticCompoundShapeSettings()
        two_mesh_shape_small_angle.add_shape(Vec3(1, 0, 0), Quat.rotation(Vec3.axis_z(), math.degrees_to_radians(2)), mesh_shape)
        two_mesh_shape_small_angle.add_shape(Vec3(-1, 0, 0), Quat.rotation(Vec3.axis_z(), math.degrees_to_radians(-2)), mesh_shape)
        
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(two_mesh_shape_small_angle, Vec3(0, 1, -4), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)
        
        box_settings = pyjolt.BodyCreationSettings(box_shape, Vec3(0, 4, -4), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        box_settings.allow_sleeping = False
        body_interface.create_and_add_body(box_settings, pyjolt.EActivation.ACTIVATE)

        two_mesh_shape_large_angle = pyjolt.StaticCompoundShapeSettings()
        two_mesh_shape_large_angle.add_shape(Vec3(1, 0, 0), Quat.rotation(Vec3.axis_z(), math.degrees_to_radians(3)), mesh_shape)
        two_mesh_shape_large_angle.add_shape(Vec3(-1, 0, 0), Quat.rotation(Vec3.axis_z(), math.degrees_to_radians(-3)), mesh_shape)

        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(two_mesh_shape_large_angle, Vec3(0, 1, -8), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)

        box_settings = pyjolt.BodyCreationSettings(box_shape, Vec3(0, 4, -8), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        box_settings.allow_sleeping = False
        body_interface.create_and_add_body(box_settings, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()