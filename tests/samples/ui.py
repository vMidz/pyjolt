import pygame
import moderngl as mgl
from settings import Settings
from shaders_utility import Text
from imgui_bundle import imgui, imgui_ctx
from tests.test_common import start_test, Context

from tests.general.activate_during_update_test import ActivateDuringUpdateTest
from tests.general.active_edges_test import ActiveEdgesTest
from tests.general.allowed_do_fs_test import AllowedDOFsTest
from tests.general.big_vs_small_test import BigVsSmallTest
from tests.general.center_of_mass_test import CenterOfMassTest
from tests.general.change_motion_quality_test import ChangeMotionQualityTest
from tests.general.change_motion_type_test import ChangeMotionTypeTest
from tests.general.change_object_layer_test import ChangeObjectLayerTest
from tests.general.change_shape_test import ChangeShapeTest
from tests.general.contact_listener_test import ContactListenerTest
from tests.general.contact_manifold_test import ContactManifoldTest
from tests.general.conveyor_belt_test import ConveyorBeltTest
from tests.general.damping_test import DampingTest
from tests.general.dynamic_mesh_test import DynamicMeshTest
from tests.general.enhanced_internal_edge_removal_test import EnhancedInternalEdgeRemovalTest
from tests.general.friction_per_triangle_test import FrictionPerTriangleTest
from tests.general.friction_test import FrictionTest
from tests.general.funnel_test import FunnelTest                   
from tests.general.gravity_factor_test import GravityFactorTest
from tests.general.gyroscopic_force_test import GyroscopicForceTest
from tests.general.heavy_on_light_test import HeavyOnLightTest
# from tests.general.high_speed_test import high_speed_test              # TODO:
from tests.general.island_test import IslandTest
from tests.general.kinematic_test import KinematicTest
from tests.general.load_save_binary_test import LoadSaveBinaryTest
# from tests.general.load_save_scene_test import LoadSaveSceneTest       # TODO:
from tests.general.manifold_reduction_test import ManifoldReductionTest
from tests.general.modify_mass_test import ModifyMassTest
# from tests.general.multithreaded_test import MultithreadedTest         # Not supported
from tests.general.pyramid_test import PyramidTest
from tests.general.restitution_test import RestitutionTest
# from tests.general.sensor_test import sensorTest                       # TODO:
from tests.general.shape_filter_test import ShapeFilterTest
from tests.general.simple_test import SimpleTest
from tests.general.stack_test import StackTest
from tests.general.two_d_funnel_test import TwoDFunnelTest
from tests.general.wall_test import WallTest

from imgui_bundle.python_backends.opengl_backend_programmable import ProgrammablePipelineRenderer
from pygameBackend import PygameRenderer # copied from the merged base
# from imgui_bundle.python_backends.python_backends_disabled.pygame_backend import PygameRenderer  # Has import bug   was fixed recently in pull request

class UI:
    def __init__(self, ctx: mgl.Context, width: int, height: int,
                 physics_system, settings: Settings, test_context: Context, pending_text: list[Text]):
        self.physics_system = physics_system
        self.settings = settings

        imgui.create_context()

        self.pending_text = pending_text
        self.test_context = test_context
        self.mgl_ctx = ctx
        self.imgui_renderer = ProgrammablePipelineRenderer()
        self.imgui_input_handler = PygameRenderer()
        self.main_camera = test_context.main_camera

        io = imgui.get_io()
        io.display_size = (width,height)

        self.show_help = False
        self.MENU_STRUCTURE = {
        "Select Test": {
            "General": [
                ("Simple Test", lambda: start_test(self.test_context, SimpleTest)),
                ("Stack", lambda: start_test(self.test_context, StackTest)),
                ("WallTest", lambda: start_test(self.test_context, WallTest)),
                ("TwoD Funnel Test", lambda: start_test(self.test_context, TwoDFunnelTest)),
                ("Center Of Mass Test", lambda: start_test(self.test_context, CenterOfMassTest)),
                ("Active Edges Test", lambda: start_test(self.test_context, ActiveEdgesTest)),
                ("Big Vs Small Test", lambda: start_test(self.test_context, BigVsSmallTest)),
                ("Activate During Update Test", lambda: start_test(self.test_context, ActivateDuringUpdateTest)),
                ("Allowed DOFs Test", lambda: start_test(self.test_context, AllowedDOFsTest)),
                ("Change Motion Quality Test", lambda: start_test(self.test_context, ChangeMotionQualityTest)),
                ("Change Motion Type Test", lambda: start_test(self.test_context, ChangeMotionTypeTest)),
                ("Change ObjectLayer Test", lambda: start_test(self.test_context, ChangeObjectLayerTest)),
                ("Change Shape Test", lambda: start_test(self.test_context, ChangeShapeTest)),
                ("Contact Listener", lambda: start_test(self.test_context, ContactListenerTest)),
                ("Contact Manifold Test", lambda: start_test(self.test_context, ContactManifoldTest)),
                ("Damping Test", lambda: start_test(self.test_context, DampingTest)),
                ("Conveyor Belt Test", lambda: start_test(self.test_context, ConveyorBeltTest)),
                ("Dynamic Mesh Test", lambda: start_test(self.test_context, DynamicMeshTest)),
                ("Enhanced Internal EdgeRemoval Test", lambda: start_test(self.test_context, EnhancedInternalEdgeRemovalTest)),
                ("Friction Test", lambda: start_test(self.test_context, FrictionTest)),
                ("Island Test", lambda: start_test(self.test_context, IslandTest)),
                ("Friction Per Triangle Test", lambda: start_test(self.test_context, FrictionPerTriangleTest)),
                ("Funnel Test", lambda: start_test(self.test_context, FunnelTest)),
                ("Kinematic Test", lambda: start_test(self.test_context, KinematicTest)),
                ("Gravity Factor Test", lambda: start_test(self.test_context, GravityFactorTest)),
                ("Gyroscopic Force Test", lambda: start_test(self.test_context, GyroscopicForceTest)),
                ("Heavy On Light Test", lambda: start_test(self.test_context, HeavyOnLightTest)),
                ("Manifold Reduction Test", lambda: start_test(self.test_context, ManifoldReductionTest)),
                ("Restitution Test", lambda: start_test(self.test_context, RestitutionTest)),
                ("Pyramid Test", lambda: start_test(self.test_context, PyramidTest)),
                ("Modify Mass Test", lambda: start_test(self.test_context, ModifyMassTest)),
                ("Shape Filter Test", lambda: start_test(self.test_context, ShapeFilterTest)),
                ("Load Save Binary Test", lambda: start_test(self.test_context, LoadSaveBinaryTest)),
            ],
            "Shapes": [
                ("Box Shape Test", lambda: print("Box Shape Test")),
            ]
        },
            "Test Settings": lambda: self.place_holder(),
            "Restart Test": lambda: self.place_holder(),
            "Run All Tests": lambda: self.place_holder(),
            "Next Test": lambda: self.place_holder(),
            "Take Snapshot": lambda: self.place_holder(),
            "Take And Reload Snapshot": lambda: self.place_holder(),
            "Physics Settings": lambda: self.place_holder(),
            "Drawing Options": lambda: setattr(self.settings, 'show_menu', True),
            "Mouse Probe": lambda: self.place_holder(),
            "Shoot Object": lambda: self.place_holder(),
            "Help": lambda: setattr(self, 'show_help', True),
        }

    def place_holder(self):
        ...

    def render_menu_item(self, name, action):
        if imgui.menu_item_simple(name):
            action()

    def render_menu_level(self, menu_dict):
        for name, content in menu_dict.items():
            if callable(content):
                self.render_menu_item(name, content)
            elif isinstance(content, dict):
                if imgui.begin_menu(name):
                    self.render_menu_level(content)
                    imgui.end_menu()
            elif isinstance(content, list):
                if imgui.begin_menu(name):
                    for item_name, action in content:
                        self.render_menu_item(item_name, action)
                    imgui.end_menu()

    def process_event(self, event):
        self.imgui_input_handler.process_event(event)

    def update(self, dt: float, clock: pygame.time.Clock):
        self.imgui_input_handler.process_inputs()

        self.mgl_ctx.screen.use()
        
        imgui.new_frame()

        with imgui_ctx.begin_main_menu_bar():
            with imgui_ctx.begin_menu("Main") as menu:
                if menu.visible:
                    self.render_menu_level(self.MENU_STRUCTURE)

            if self.settings.show_menu:
                self.settings.show_drawing_options_menu()

            SPACE = '\t' * 5
            imgui.text(f'{SPACE}FPS: {clock.get_fps():.2f}')

            draw_list = imgui.get_background_draw_list()
            draw_list.add_text(imgui.ImVec2(10, 40), imgui.get_color_u32(imgui.ImVec4(1, 1, 1, 1)), "Press ALT to show/hide cursor")

            # 3D text
            BASE_PIXEL_SIZE = 16.0 
            REFERENCE_W = 50.0

            draw_list = imgui.get_foreground_draw_list()

            font = imgui.get_font()

            for text in self.pending_text:
                desired_pixel_size = (text.height * BASE_PIXEL_SIZE * REFERENCE_W) / text.w
                
                desired_pixel_size = max(8.0, min(desired_pixel_size, 72.0))

                imgui.push_font(font, desired_pixel_size)
                
                text_size = imgui.calc_text_size(text.string)
                
                centered_pos = (
                    text.screen_pos.x - text_size.x * 0.5,
                    text.screen_pos.y - text_size.y * 0.5
                )
                
                draw_list.add_text(centered_pos, text.imgui_clr, text.string)

                imgui.pop_font()

            self.pending_text.clear()

        if self.show_help:
            with imgui_ctx.begin("Help window", True) as custom_window:
                self.show_help = custom_window.opened
                if custom_window.expanded:
                    imgui.text("ESC: Back to previous menu.\n"
                               "WASD + Mouse: Fly around. Hold Shift to speed up, Ctrl to slow down.\n"
                               "Space: Hold to pick up and drag a physics object under the crosshair.\n"
                               "P: Pause / unpause simulation.\n"
                               "O: Single step the simulation.\n"
                               ",: Step back (only when Physics Settings / Record State for Playback is on).\n"
                               ".: Step forward (only when Physics Settings / Record State for Playback is on).\n"
                               "Shift + ,: Play reverse (only when Physics Settings / Record State for Playback is on).\n"
                               "Shift + .: Replay forward (only when Physics Settings / Record State for Playback is on).\n"
                               "T: Dump frame timing information to profile_*.html (when JPH_PROFILE_ENABLED defined).")

                    if imgui.button("Hello"):
                        print("World")
           
                    io = imgui.get_io()
                    imgui.text(f"""
                    Keyboard modifiers:
                        {io.key_ctrl=}
                        {io.key_alt=}
                        {io.key_shift=}
                        {io.key_super=}""")

                    if imgui.button("Open popup"):
                        imgui.open_popup("my popup")
                    with imgui_ctx.begin_popup("my popup") as popup:
                        if popup.visible:
                            imgui.text("Hello from popup!")
                            if imgui.button("Close popup"):
                                imgui.close_current_popup()

        imgui.render()
        self.imgui_renderer.render(imgui.get_draw_data())
