from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math, Color
from pyjolt.math import Vec3, Quat, Mat44
import enum
from math import fmod

class ShapeIdentifier(enum.IntEnum):
    Box = 1
    Sphere = 2
    Compound = 3

class MyShapeFilter(pyjolt.ShapeFilter):
    def __init__(self):
        super().__init__()
        self.user_data_of_shape_to_ignore = ShapeIdentifier.Sphere

    def should_collide(self, shape1: pyjolt.Shape, sub_shape_id1: pyjolt.SubShapeID, shape2: pyjolt.Shape, sub_shape_id2: pyjolt.SubShapeID):
        return shape1.get_user_data() != self.user_data_of_shape_to_ignore

class ShapeFilterTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        self.elapsed_time = 0.0
        self.cast_shape = None

        # Create geometry to cast against
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            pyjolt.BoxShape(Vec3(20, 1, 3)), Vec3(0, -1, 0), Quat.identity(), 
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.ACTIVATE)
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            pyjolt.BoxShape(Vec3.replicate(3)), Vec3(0, 3, 0), Quat.identity(), 
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.ACTIVATE)

        # Create shape to cast
        box_shape_settings = pyjolt.BoxShapeSettings(Vec3.replicate(1))
        box_shape_settings.user_data = ShapeIdentifier.Box

        sphere_shape_settings = pyjolt.SphereShapeSettings(1)
        sphere_shape_settings.user_data = ShapeIdentifier.Sphere

        cast_shape_settings = pyjolt.StaticCompoundShapeSettings()
        cast_shape_settings.add_shape(Vec3(3, 2, 0), Quat.identity(), box_shape_settings)
        cast_shape_settings.add_shape(Vec3(0, 0, 0), Quat.identity(), sphere_shape_settings)
        cast_shape_settings.user_data = ShapeIdentifier.Compound

        self.cast_shape = cast_shape_settings.create().get()

    def post_physics_update(self, delta_time: float) -> None:
        self.elapsed_time += delta_time
        phase = self.elapsed_time

        cast_origin = Vec3(math.cos(phase) * 10, 10, 0)
        cast_motion = Vec3(0, -15, 0)

        collector = pyjolt.ClosestHitCollisionCollector_CastShapeCollector()

        shape_filter = MyShapeFilter()

        # Select which shape to ignore
        shape_select = fmod(phase, 6.0 * math.JPH_PI)
        text = ""
        if shape_select < 2.0 * math.JPH_PI:
            shape_filter.user_data_of_shape_to_ignore = ShapeIdentifier.Box
            text = "Box"
        elif shape_select < 4.0 * math.JPH_PI:
            shape_filter.user_data_of_shape_to_ignore = ShapeIdentifier.Sphere
            text = "Sphere"
        else:
            shape_filter.user_data_of_shape_to_ignore = ShapeIdentifier.Compound
            text = "Compound"
        
        # TODO:
        pyjolt.DebugRenderer.instance.draw_text3d(cast_origin, f"Ignoring shape: {text}", pyjolt.Color.White)

        narrow_phase_query = self.physics_system.get_narrow_phase_query()
        shape_cast = pyjolt.RShapeCast(self.cast_shape, Vec3.replicate(1), Mat44.create_translation_matrix(cast_origin), cast_motion)
        narrow_phase_query.cast_shape(
            shape_cast,
            pyjolt.ShapeCastSettings(),
            Vec3.zero(),
            collector,
            pyjolt.BroadPhaseLayerFilter(),
            pyjolt.ObjectLayerFilter(),
            pyjolt.BodyFilter(), 
            shape_filter
        )
 
        cast_point = Vec3()
        color = Color()
        if collector.had_hit():
            cast_point = cast_origin + cast_motion * collector.hit.fraction
            color = pyjolt.Color.Green
        else:
            cast_point = cast_origin + cast_motion
            color = pyjolt.Color.Red

        pyjolt.DebugRenderer.instance.draw_arrow(cast_origin, cast_point, pyjolt.Color.Orange, 0.1)
        self.cast_shape.draw(pyjolt.DebugRenderer.instance, Mat44.create_translation_matrix(cast_point), Vec3.replicate(1.0), color, False, True)
    
    def cleanup(self):
        super().cleanup()