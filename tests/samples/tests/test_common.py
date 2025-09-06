import pyjolt
from pyjolt.math import Vec3, Quat, Mat44, Float3
from pyjolt import math
from layers import *
from camera import Camera

current_test = None

class Context:
    def __init__(self, physics_system: pyjolt.PhysicsSystem, main_camera: Camera):
        self.physics_system = physics_system
        self.main_camera = main_camera
        
def pre_physics_update(deltaTime):
    global current_test
    if hasattr(current_test, "pre_physics_update"):
        current_test.pre_physics_update(deltaTime)

def post_physics_update(deltaTime):
    global current_test
    if hasattr(current_test, "post_physics_update"):
        current_test.post_physics_update(deltaTime)

def start_test(context: Context, new_test_class):
    global current_test
    
    if hasattr(current_test, "set_starting_camera_position"):
        current_test.set_starting_camera_position()
    else:
        context.main_camera.reset()

    current_test = new_test_class(context)
    context.physics_system.optimize_broad_phase()

class Test:
    def __init__(self, ctx: Context):
        global current_test

        self.ctx = ctx
        self.physics_system = ctx.physics_system

        if current_test:
            current_test.cleanup()

    def create_floor(self, size: float = 200.0):
        body_interface = self.physics_system.get_body_interface()
        floor = body_interface.create_body(pyjolt.BodyCreationSettings(pyjolt.BoxShape(Vec3(0.5 * size, 1.0, 0.5 * size), 0.0), (Vec3(0.0, -1.0, 0.0)), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING))
        body_interface.add_body(floor.get_id(), pyjolt.EActivation.DONT_ACTIVATE)
        return floor

    def cleanup(self):
        bi = self.physics_system.get_body_interface()
        bodies = self.physics_system.get_bodies()
        for body in bodies:
            bi.remove_body(body)
            bi.destroy_body(body)

class ShapeCreator:
    @staticmethod
    def create_torus_mesh(
        torus_radius: float, 
        tube_radius: float, 
        torus_segments: int = 20, 
        tube_segments: int = 12
    ) -> pyjolt.Shape:
        """
        Creates a torus mesh shape.
        """
        num_vertices = torus_segments * tube_segments

        # Create torus mesh settings
        mesh_settings = pyjolt.MeshShapeSettings()

        for torus_segment in range(torus_segments):
            torus_angle = float(torus_segment) * 2.0 * math.JPH_PI / torus_segments
            rotation = Mat44.create_rotation_matrix(Vec3.axis_y(), torus_angle)
            
            for tube_segment in range(tube_segments):
                # Create vertices
                tube_angle = float(tube_segment) * 2.0 * math.JPH_PI / tube_segments
                pos = rotation * Vec3(
                    torus_radius + tube_radius * math.sin(tube_angle),
                    tube_radius * math.cos(tube_angle),
                    0.0
                )
                
                # Jolt uses Float3 for triangle vertices
                v = Float3(pos.x, pos.y, pos.z)
                mesh_settings.triangle_vertices.push_back(v)

                # Create indices for the two triangles that form a quad
                start_idx = torus_segment * tube_segments + tube_segment
                
                # First triangle
                idx1 = start_idx
                idx2 = (start_idx + 1) % num_vertices
                idx3 = (start_idx + tube_segments) % num_vertices
                mesh_settings.indexed_triangles.push_back(pyjolt.IndexedTriangle(idx1, idx2, idx3))
                
                # Second triangle
                idx4 = (start_idx + 1) % num_vertices
                idx5 = (start_idx + tube_segments + 1) % num_vertices
                idx6 = (start_idx + tube_segments) % num_vertices
                mesh_settings.indexed_triangles.push_back(pyjolt.IndexedTriangle(idx4, idx5, idx6))
        
        # Create the shape from the settings and return it
        # .create() returns a ShapeResult, .get() extracts the shape
        return mesh_settings.create().get()
