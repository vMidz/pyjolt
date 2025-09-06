from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat
import math

class AllowedDOFsTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()
        
        self.bodies = pyjolt.BodyIDArray()

        # Floor
        self.create_floor()

        box_size = Vec3(0.5, 1.0, 2.0)
        box_shape = pyjolt.BoxShape(box_size)

        for allowed_dofs in range(1, 0b111111 + 1):
            x = -35.0 + 10.0 * (allowed_dofs & 0b111)
            z = -35.0 + 10.0 * ((allowed_dofs >> 3) & 0b111)

            # Create body
            bcs = pyjolt.BodyCreationSettings(
                box_shape, Vec3(x, 10, z), Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, Layers.MOVING)

            bcs.set_shape_allowed_dof(allowed_dofs)
            body_id = body_interface.create_and_add_body(bcs, pyjolt.EActivation.ACTIVATE)
            # self.bodies.push_back(body_id)

            # Create a constraint
            dcs = pyjolt.DistanceConstraintSettings()
            dcs.point1 = bcs.position + Vec3(5, 5, 5)
            dcs.point2 = bcs.position + box_size
            dcs.min_distance = 0.0
            dcs.max_distance = math.sqrt(3.0) * 5.0 + 1.0
            
            constraint = body_interface.create_constraint(dcs, pyjolt.BodyID(), body_id)
            self.physics_system.add_constraint(constraint)

    # TODO: post_physics_update
    def post_physics_update(self, delat_time):
        ...

    def cleanup(self):
        constraints = self.physics_system.get_constraints()
        for constraint in constraints:
            self.physics_system.remove_constraint(constraint.get())
        super().cleanup()
