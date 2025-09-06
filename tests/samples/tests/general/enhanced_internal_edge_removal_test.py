from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math, Triangle
from pyjolt.math import Vec3, Quat, Float3

class EnhancedInternalEdgeRemovalTest(Test):
    def create_sliding_objects(self, in_start: Vec3):
        body_interface = self.physics_system.get_body_interface()
        pos = in_start - Vec3(0, 0, 12.0)
        for enhanced_removal in range(2):
            is_enhanced = (enhanced_removal == 1)

            # A box
            box_bcs = pyjolt.BodyCreationSettings(
                pyjolt.BoxShape(Vec3.replicate(2.0)), pos, Quat.identity(),
                pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
            box_bcs.linear_velocity = Vec3(20, 0, 0)
            box_bcs.enhanced_internal_edge_removal = is_enhanced
            body_interface.create_and_add_body(box_bcs, pyjolt.EActivation.ACTIVATE)
            pos += Vec3(0, 0, 5.0)

            # A sphere
            sphere_bcs = pyjolt.BodyCreationSettings(
                pyjolt.SphereShape(2.0), pos, Quat.identity(),
                pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
            sphere_bcs.linear_velocity = Vec3(20, 0, 0)
            sphere_bcs.enhanced_internal_edge_removal = is_enhanced
            body_interface.create_and_add_body(sphere_bcs, pyjolt.EActivation.ACTIVATE)
            pos += Vec3(0, 0, 5.0)

            # Compound
            box = pyjolt.BoxShape(Vec3.replicate(0.1))
            compound_settings = pyjolt.StaticCompoundShapeSettings()
            for x in range(2):
                for y in range(2):
                    for z in range(2):
                        shape_pos = Vec3(
                            -1.9 if x == 0 else 1.9,
                            -1.9 if y == 0 else 1.9,
                            -1.9 if z == 0 else 1.9
                        )
                        compound_settings.add_shape(shape_pos, Quat.identity(), box)
            
            compound_bcs = pyjolt.BodyCreationSettings(
                compound_settings, pos, Quat.identity(),
                pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
            compound_bcs.linear_velocity = Vec3(20, 0, 0)
            compound_bcs.enhanced_internal_edge_removal = is_enhanced
            body_interface.create_and_add_body(compound_bcs, pyjolt.EActivation.ACTIVATE)
            pos += Vec3(0, 0, 7.0)

    def __init__(self, physics_system: pyjolt.PhysicsSystem):
        super().__init__(physics_system)
        body_interface = self.physics_system.get_body_interface()
        self.level_ball = None

        # This test creates a grid of connected boxes
        compound_settings_1 = pyjolt.StaticCompoundShapeSettings()
        size = 2.0
        box_shape = pyjolt.BoxShape(Vec3.replicate(0.5 * size))
        for x in range(-10, 10):
            for z in range(-10, 10):
                compound_settings_1.add_shape(Vec3(size * x, 0, size * z), Quat.identity(), box_shape)
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            compound_settings_1, Vec3(0, -1, -40), Quat.identity(),
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)
        self.create_sliding_objects(Vec3(-18, 1.9, -40.0))

        # This tests if objects do not collide with internal edges of a mesh
        triangles_1 = pyjolt.TriangleArray()
        for x in range(-10, 10):
            for z in range(-10, 10):
                x1, z1 = size * x, size * z
                x2, z2 = x1 + size, z1 + size
                v1, v2 = Float3(x1, 0, z1), Float3(x2, 0, z1)
                v3, v4 = Float3(x1, 0, z2), Float3(x2, 0, z2)
                triangles_1.push_back(Triangle(v1, v3, v4))
                triangles_1.push_back(Triangle(v1, v4, v2))
        
        mesh_settings_1 = pyjolt.MeshShapeSettings(triangles_1)
        mesh_settings_1.active_edge_cos_threshold_angle = float('inf') # Turn off regular active edge determination
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            mesh_settings_1, Vec3.zero(), Quat.identity(),
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)
        self.create_sliding_objects(Vec3(-18, 1.9, 0))

        # This test tests that we only ignore edges that are shared with voided triangles
        triangles_2 = pyjolt.TriangleArray()
        height, half_width, half_length = 0.5, 5.0, 2.0
        triangles_2.push_back(Triangle(Float3(-half_length, 0, half_width), Float3(half_length, 0, -half_width), Float3(-half_length, 0, -half_width)))
        triangles_2.push_back(Triangle(Float3(-half_length, 0, half_width), Float3(half_length, 0, half_width), Float3(half_length, 0, -half_width)))
        triangles_2.push_back(Triangle(Float3(half_length, height, half_width), Float3(half_length, height, -half_width), Float3(half_length, 0, half_width)))
        triangles_2.push_back(Triangle(Float3(half_length, 0, half_width), Float3(half_length, height, -half_width), Float3(half_length, 0, -half_width)))
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(pyjolt.MeshShapeSettings(triangles_2), Vec3(0, 0, 30), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)

        z_pos = 28.0
        for enhanced_removal in range(2):
            sphere_bcs = pyjolt.BodyCreationSettings(pyjolt.SphereShape(1.0), Vec3(0, 1, z_pos), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
            sphere_bcs.linear_velocity = Vec3(20, 0, 0)
            sphere_bcs.enhanced_internal_edge_removal = (enhanced_removal == 1)
            body_interface.create_and_add_body(sphere_bcs, pyjolt.EActivation.ACTIVATE)
            z_pos += 4.0

        # This tests that fast moving spheres rolling over a triangle will not be affected by internal edges
        plane_triangles = pyjolt.TriangleArray()
        plane_triangles.push_back(Triangle(Float3(-10, 0, -10), Float3(-10, 0, 10), Float3(10, 0, 10)))
        plane_triangles.push_back(Triangle(Float3(-10, 0, -10), Float3(10, 0, 10), Float3(10, 0, -10)))
        plane_mesh = pyjolt.MeshShapeSettings(plane_triangles)

        level_plane = pyjolt.BodyCreationSettings(plane_mesh, Vec3(-10, 0, 50), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING)
        level_plane.friction = 1.0
        body_interface.create_and_add_body(level_plane, pyjolt.EActivation.DONT_ACTIVATE)

        level_ball = pyjolt.BodyCreationSettings(pyjolt.SphereShape(0.5), Vec3(-10, 1, 41), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        level_ball.enhanced_internal_edge_removal = True
        level_ball.friction = 1.0
        level_ball.override_mass_properties = pyjolt.EOverrideMassProperties.CALCULATE_INERTIA
        level_ball.mass_properties_override.mass = 1.0
        self.level_ball = body_interface.create_and_add_body(level_ball, pyjolt.EActivation.ACTIVATE)

        slope_plane = pyjolt.BodyCreationSettings(plane_mesh, Vec3(10, 0, 50), Quat.rotation(Vec3.axis_x(), math.degrees_to_radians(45)), pyjolt.EMotionType.STATIC, Layers.NON_MOVING)
        slope_plane.friction = 1.0
        body_interface.create_and_add_body(slope_plane, pyjolt.EActivation.DONT_ACTIVATE)

        slope_ball = pyjolt.BodyCreationSettings(pyjolt.SphereShape(0.5), Vec3(10, 8, 44), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        slope_ball.enhanced_internal_edge_removal = True
        slope_ball.friction = 1.0
        slope_ball.override_mass_properties = pyjolt.EOverrideMassProperties.CALCULATE_INERTIA
        slope_ball.mass_properties_override.mass = 1.0
        body_interface.create_and_add_body(slope_ball, pyjolt.EActivation.ACTIVATE)

        # This tests a previous bug where a compound shape will fall through a box
        box_bcs = pyjolt.BodyCreationSettings(pyjolt.BoxShape(Vec3.replicate(2.5)), Vec3(0, 0, 70), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING)
        body_interface.create_and_add_body(box_bcs, pyjolt.EActivation.DONT_ACTIVATE)
        
        compound_settings_2 = pyjolt.StaticCompoundShapeSettings()
        compound_settings_2.add_shape(Vec3(-2.5, 0, 0), Quat.identity(), pyjolt.BoxShape(Vec3(2.5, 0.1, 0.1)))
        compound_settings_2.add_shape(Vec3(0.1, 0, 0), Quat.identity(), pyjolt.BoxShape(Vec3(0.1, 1.0, 1.0)))
        compound_bcs = pyjolt.BodyCreationSettings(compound_settings_2, Vec3(2, 5, 70), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        compound_bcs.enhanced_internal_edge_removal = True
        body_interface.create_and_add_body(compound_bcs, pyjolt.EActivation.ACTIVATE)

    def pre_physics_update(self, delta_time) -> None:
        # Increase rotation speed of the ball on the flat plane
        body_interface = self.physics_system.get_body_interface()
        body_interface.add_torque(self.level_ball, Vec3(math.JPH_PI * 4, 0, 0))

    def cleanup(self):
        super().cleanup()