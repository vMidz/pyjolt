from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat, Float3

class BigVsSmallTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Create a big triangle
        triangles = pyjolt.TriangleArray()
        triangles.push_back(pyjolt.Triangle(Float3(-100, 0, 0), Float3(0, 0, 100), Float3(100, 0, -100)))
        
        mesh_settings = pyjolt.MeshShapeSettings(triangles)
        
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            mesh_settings, Vec3.zero(), Quat.identity(), 
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)

        # A small box
        box_settings = pyjolt.BodyCreationSettings(
            pyjolt.BoxShape(Vec3(0.1, 0.1, 0.1)), Vec3(0, 1.0, 0), 
            Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
            
        body = body_interface.create_body(box_settings)
        body.allow_sleeping = False
        body_interface.add_body(body.get_id(), pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()