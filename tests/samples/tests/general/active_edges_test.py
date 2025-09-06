from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class ActiveEdgesTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        c_width = 5.0
        c_length = 10.0

        # Settings for a frictionless box
        box_shape = pyjolt.BoxShape(Vec3(1.0, 1.0, 1.0), pyjolt.DEFAULT_CONVEX_RADIUS)
        box_settings = pyjolt.BodyCreationSettings(
            box_shape, Vec3.zero(), Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        box_settings.friction = 0.0
        box_settings.linear_damping = 0.0
        box_settings.allow_sleeping = False

        # Create various triangle strips
        triangles = pyjolt.TriangleArray()
        for angle in range(-90, 91):
            # Under which normal we want to place the block
            if angle < 0:
                desired_normal = Vec3(0, 1, -1).normalized()
            else:
                desired_normal = Vec3(0, 1, 0)
            best_dot = -float('inf')

            # Place segments
            x = c_width * angle
            v1 = Vec3(x, 0.0, -0.5 * c_length)
            v2 = Vec3(x + c_width, 0.0, -0.5 * c_length)
            
            total_angle = 0
            cur_segment = 0
            while abs(total_angle) <= 90 and cur_segment < 90:
                # Determine positions of end of this segment
                total_angle_rad = math.degrees_to_radians(float(total_angle))
                rotation = Quat.rotation(Vec3.axis_x(), total_angle_rad)
                delta = c_length * (rotation * Vec3.axis_z())
                v3 = v1 + delta
                v4 = v2 + delta

                # Check if this segment is the best segment to place the dynamic block on
                normal = (v3 - v1).cross(v2 - v1).normalized()
                dot = normal.dot(desired_normal)
                if dot > best_dot:
                    best_dot = dot
                    box_settings.position = Vec3((v1 + v2 + v3 + v4) / 4.0 + normal)
                    box_settings.rotation = rotation

                # Add segment
                triangles.push_back(pyjolt.Triangle(v1, v3, v4))
                triangles.push_back(pyjolt.Triangle(v1, v4, v2))

                # Add segment mirrored in Z axis
                if cur_segment != 0:
                    flip = Vec3(1, 1, -1)
                    triangles.push_back(pyjolt.Triangle(flip * v1, flip * v4, flip * v3))
                    triangles.push_back(pyjolt.Triangle(flip * v1, flip * v2, flip * v4))

                # The end of the segment will be the start for the next iteration
                v1 = v3
                v2 = v4
                
                # Increment loop variables
                total_angle += angle
                cur_segment += 1

            # Place box on best segment
            body = body_interface.create_body(box_settings)
            body_interface.add_body(body.get_id(), pyjolt.EActivation.ACTIVATE)

            # For convex segments give the block a push
            if angle >= 0:
                body.linear_velocity = Vec3(0, 0, 2.0)

        # Mesh shape
        mesh_shape_settings = pyjolt.MeshShapeSettings(triangles)
        mesh_shape_settings.active_edge_cos_threshold_angle = math.cos(math.degrees_to_radians(50.0))

        # Mesh
        mesh_settings = pyjolt.BodyCreationSettings(
            mesh_shape_settings, Vec3.zero(), Quat.identity(), 
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING)
        # Instead of setting mActiveEdgeCosThresholdAngle you can also set: 
        # mesh_settings.enhanced_internal_edge_removal = True
        mesh_settings.friction = 0.0
        body_interface.create_and_add_body(mesh_settings, pyjolt.EActivation.DONT_ACTIVATE)

    def cleanup(self):
        super().cleanup()