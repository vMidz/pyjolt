from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math, Body, SubShapeID, Triangle, Color, ContactSettings, ContactManifold
from pyjolt.math import Vec3, Quat, Float3
from math import sqrt

class MyMaterial(pyjolt.PhysicsMaterialSimple):
    def __init__(self, debug_name: str, debug_color: Color, friction: float, restitution: float):
        super().__init__()
        self.debug_name = debug_name
        self.debug_color = debug_color
        self.friction = friction
        self.restitution = restitution

class ContactListenerImpl(pyjolt.ContactListener):
    def __init__(self, test_instance: 'FrictionPerTriangleTest'):
        super().__init__()
        self.test_instance = test_instance

    def on_contact_added(self, body1: Body, body2: Body, manifold: ContactManifold, settings: ContactSettings) -> None:
        self.test_instance.s_override_contact_settings(body1, body2, manifold, settings)

    def on_contact_persisted(self, body1: Body, body2: Body, manifold: ContactManifold, settings: ContactSettings) -> None:
        self.test_instance.s_override_contact_settings(body1, body2, manifold, settings)

class FrictionPerTriangleTest(Test):
    @staticmethod
    def s_get_friction_and_restitution(body: Body, sub_shape_id: SubShapeID) -> tuple[float, float]:
        material = body.get_shape().get_material(sub_shape_id)

        # Check if it's our custom material type
        if isinstance(material, MyMaterial):
            return material.friction, material.restitution
        else:
            return body.friction, body.restitution

    @staticmethod
    def s_override_contact_settings(body1: Body, body2: Body, manifold: ContactManifold, settings: ContactSettings) -> None:
        # Get the custom friction and restitution for both bodies
        friction1, restitution1 = FrictionPerTriangleTest.s_get_friction_and_restitution(body1, manifold.sub_shape_id1)
        friction2, restitution2 = FrictionPerTriangleTest.s_get_friction_and_restitution(body2, manifold.sub_shape_id2)

        # Use the default formulas for combining friction and restitution
        settings.combined_friction = sqrt(friction1 * friction2)
        settings.combined_restitution = max(restitution1, restitution2)

    def __init__(self, ctx: Context):
        super().__init__(ctx)

        self.contact_listner = ContactListenerImpl(self)

        body_interface = self.physics_system.get_body_interface()
        
        num_sections = 5
        section_size = 50.0

        # Create a strip of triangles
        triangles = pyjolt.TriangleArray()
        for z in range(num_sections + 1):
            z1 = section_size * (z - 0.5 * num_sections)
            z2 = z1 + section_size

            v1 = Float3(-100.0, 0, z1)
            v2 = Float3(100.0, 0, z1)
            v3 = Float3(-100.0, 0, z2)
            v4 = Float3(100.0, 0, z2)
            
            # The last argument to Triangle is the material index
            triangles.push_back(Triangle(v1, v3, v4, z))
            triangles.push_back(Triangle(v1, v4, v2, z))

        # Create materials with increasing friction
        materials = pyjolt.PhysicsMaterialListArray()
        for i in range(num_sections + 1):
            friction = float(i) / float(num_sections)
            materials.push_back(pyjolt.PhysicsMaterialRefConst())
            # TODO: materials
            # materials.push_back(MyMaterial(f"Friction {friction}", Color.get_distinct_color(i), friction, 0.0))

        # A ramp
        mesh_settings = pyjolt.MeshShapeSettings(triangles, materials)
        ramp_settings = pyjolt.BodyCreationSettings(
            mesh_settings,
            Vec3.zero(), 
            Quat.rotation(Vec3.axis_x(), 0.2 * math.JPH_PI), 
            pyjolt.EMotionType.STATIC, 
            Layers.NON_MOVING
        )
        body_interface.create_and_add_body(ramp_settings, pyjolt.EActivation.DONT_ACTIVATE)

        # A box with friction 1 that slides down the ramp
        # box_material = MyMaterial("Box Friction 1", Color.Yellow, 1.0, 0.0) # TODO:
        # box_shape = pyjolt.BoxShape(Vec3(2.0, 2.0, 2.0), pyjolt.DEFAULT_CONVEX_RADIUS, box_material)
        box_shape = pyjolt.BoxShape(Vec3(2.0, 2.0, 2.0))
        box_settings = pyjolt.BodyCreationSettings(
            box_shape, 
            Vec3(0, 60.0, -75.0),
            Quat.rotation(Vec3.axis_x(), 0.2 * math.JPH_PI), 
            pyjolt.EMotionType.DYNAMIC, 
            Layers.MOVING
        )
        body_interface.create_and_add_body(box_settings, pyjolt.EActivation.ACTIVATE)

        # Set this object as the contact listener
        ctx.physics_system.set_contact_listener(self.contact_listner)

    def cleanup(self):
        # Unset the contact listener
        self.physics_system.set_contact_listener(None)
        super().cleanup()