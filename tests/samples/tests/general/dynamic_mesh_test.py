from tests.test_common import Test, Layers, ShapeCreator, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class DynamicMeshTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        c_torus_radius = 3.0
        c_tube_radius = 1.0

        # Create torus
        mesh_shape = ShapeCreator.create_torus_mesh(c_torus_radius, c_tube_radius)
        settings = pyjolt.BodyCreationSettings(
            mesh_shape, 
            Vec3(0, 10, 0), 
            Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, 
            Layers.MOVING
        )

        # Mesh cannot calculate its mass, we must provide it
        settings.override_mass_properties = pyjolt.EOverrideMassProperties.MASS_AND_INERTIA_PROVIDED
        box_for_inertia = 2.0 * Vec3(c_torus_radius, c_tube_radius, c_torus_radius)
        settings.mass_properties_override.set_mass_and_inertia_of_solid_box(box_for_inertia, 1000.0)

        body_interface.create_and_add_body(settings, pyjolt.EActivation.ACTIVATE)

        # Wall of blocks
        box_shape = pyjolt.BoxShape(Vec3.replicate(0.5))
        for i in range(7):
            start_j = i // 2
            end_j = 7 - (i + 1) // 2
            for j in range(start_j, end_j):
                offset = 0.5 if (i & 1) else 0.0
                position = Vec3(-3.5 + j * 1.0 + offset, 0.5 + i * 1.0, 0.0)
                
                wall_settings = pyjolt.BodyCreationSettings(
                    box_shape, 
                    position, 
                    Quat.identity(), 
                    pyjolt.EMotionType.DYNAMIC, 
                    Layers.MOVING
                )
                wall = body_interface.create_body(wall_settings)
                body_interface.add_body(wall.get_id(), pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()