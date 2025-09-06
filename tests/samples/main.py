import sys
import os
import pygame as pg
import moderngl as mgl
from ui import UI
from physics import *
from camera import *
from settings import Settings
from tests.test_common import start_test, Context
from tests.general.simple_test import SimpleTest

FRAME_RATE = 60
MOVMENT_SPEED = 5.0
PHYSICS_REFRESH_RATE = 1.0/60.0
SHADOW_FAR_PLANE = 100.0
SHADOW_NEAR_PLANE = -50.0

WIDTH = 1280
HEIGHT = 720

def show_hide_cursor(state: bool) -> None:
    if state:
        pg.mouse.set_visible(True)
        pg.event.set_grab(False) 
    else:
        pg.mouse.set_visible(False)
        pg.event.set_grab(True) 

class App:
    def __init__(self, width, height):
        self.height = height
        self.width = width

        pg.init()
        pg.display.gl_set_attribute(pg.GL_CONTEXT_MAJOR_VERSION, 4)
        pg.display.gl_set_attribute(pg.GL_CONTEXT_MINOR_VERSION, 6)
        pg.display.gl_set_attribute(pg.GL_CONTEXT_PROFILE_MASK, pg.GL_CONTEXT_PROFILE_CORE)
        pg.display.gl_set_attribute(pg.GL_DOUBLEBUFFER, 1) 
        pg.display.gl_set_attribute(pg.GL_DEPTH_SIZE, 24) 

        self.surface = pg.display.set_mode((self.width, self.height), pg.OPENGL | pg.DOUBLEBUF)
        pg.display.set_caption("PyJolt")

        show_hide_cursor(False)

        self.ctx = mgl.create_context(require=460)
        self.ctx.enable(mgl.DEPTH_TEST)
        self.ctx.enable(mgl.CULL_FACE) 
        self.ctx.front_face = 'ccw' 

        self.clock = pg.time.Clock()

        self.main_camera = Camera(CAMERA_POSITION, width/height, 70.0, 0.1, 1000, width, height)

        self.view_ubo = ViewProjectionUBO(self.ctx, self.main_camera)

        self.shadow_map_cam = OrthographicCamera(Vec3(0.0, 50.0, 0.0), Vec3(50.0, 0.0, 0.0), 90, SHADOW_NEAR_PLANE, SHADOW_FAR_PLANE)
        self.light_ubo = LightUBO(self.ctx, self.shadow_map_cam)

        self.settings = Settings()
        self.physics = Physics(self.ctx, self.main_camera, self.shadow_map_cam, self.settings)

        test_context = Context(self.physics.physics_system, self.main_camera)
        
        start_test(test_context, SimpleTest)

        self.ui = UI(self.ctx, self.width, self.height, self.physics.physics_system,
                        self.settings, test_context, self.physics.debug_renderer.pending_text)

    def run(self):
        running = True
        time = 0.0
        while running:
            dt = self.clock.tick(FRAME_RATE) / 1000.0  # Ms to s
            time += dt
            for event in pg.event.get():
                if not pg.event.get_grab():
                    self.ui.process_event(event)
                if event.type == pg.QUIT:
                    running = False

                if event.type == pg.KEYDOWN:
                    if event.key == pg.K_ESCAPE:
                        running = False
                    if event.key == pg.K_LALT:
                        grabbed = pg.event.get_grab()
                        show_hide_cursor(grabbed)

                if event.type == pg.MOUSEMOTION and pg.event.get_grab():
                    self.main_camera.change_angle(Float2(event.rel[0], event.rel[1]))

            # Handle input
            speed = MOVMENT_SPEED * dt

            keys = pg.key.get_pressed()
            if keys[pg.K_LSHIFT]:
                speed *= 5
            if keys[pg.K_w]:
                self.main_camera.position += self.main_camera.forward * speed
            if keys[pg.K_s]:
                self.main_camera.position -= self.main_camera.forward * speed
            if keys[pg.K_a]:
                self.main_camera.position -= self.main_camera.right * speed
            if keys[pg.K_d]:
                self.main_camera.position += self.main_camera.right * speed
            if keys[pg.K_q]:
                self.main_camera.position -= self.main_camera.up * speed
            if keys[pg.K_e]:
                self.main_camera.position += self.main_camera.up * speed

            # Update normal cam
            self.main_camera.update_camera()

            # Update shadowmap cam
            self.shadow_map_cam.update_camera()

            # Update UBO buffers
            self.view_ubo.update_buffer()
            self.light_ubo.update_buffer()
            
            self.physics.update_and_draw(PHYSICS_REFRESH_RATE)
            self.ui.update(dt, self.clock)

            pg.display.flip()

        pg.quit()

def main():
    from pyRenderdocApp import load_render_doc # type: ignore
    from pyRenderdocApp.renderdoc_enums import RENDERDOC_CaptureOption, RENDERDOC_OverlayBits # type: ignore

    # rd = load_render_doc()                # For debugging
    # print(rd.get_api_version())
    print(os.getpid())
    try:
        app = App(WIDTH, HEIGHT)
        app.run()
    except Exception as e:
        raise e
    print("Debug: Python exit")

if __name__ == '__main__':
    main()
