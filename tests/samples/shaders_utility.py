import os
import moderngl as mgl
import numpy as np
from camera import Camera, OrthographicCamera
from pyjolt.math import Vec4, Vec3, Mat44, Float2
from dataclasses import dataclass

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
SHADER_DIR = SCRIPT_DIR + '/../assets/shaders/'

@dataclass
class Text:
    string: str
    position: Vec3
    imgui_clr: int
    height: float
    screen_pos: Float2
    w: float

def load_shader(ctx: mgl.Context, vert_shader_path: str, frag_shader_path: str) -> mgl.Program:
    HEADER='''
    #version 460 core
    const float PI = 3.14159265358979323846f;
    const float TwoPI = 2 * PI;
    layout(binding = 0, std140) uniform ViewProjection
    {
        mat4 view;
        mat4 projection;
        mat4 viewT; // view without translation
        vec4 camPos;
    };
    layout(binding = 1, std140) uniform LightingBlock
    {
        vec4 lightPosition;
        vec4 lightColor;
        mat4 lightSpace;
    };

    struct Object
    {
        mat4 model;
        mat4 inverseNormal;
        int color;
        int padding[3]; // 12 bytes
    };

    layout(binding = 4, std430) restrict readonly buffer SSBOBuffer
    {
        Object objects[];
    };
    '''

    program =ctx.program(vertex_shader = HEADER + str(_read_file(SHADER_DIR + vert_shader_path)),
                        fragment_shader = HEADER + str(_read_file(SHADER_DIR + frag_shader_path)))
    return program

def _read_file(path: str) -> str:
    file = open(path, 'r', encoding='utf-8')
    return file.read()

class ViewProjectionUBO:
    def __init__(self, ctx: mgl.Context, camera: Camera):
        self.ctx = ctx
        self.camera = camera
        self.buffer_size = 208
        self.buffer = ctx.buffer(reserve=self.buffer_size)
        self.buffer.bind_to_uniform_block(binding=0)
        
    def update_buffer(self):
        data = bytearray()
        
        data.extend(self.camera.view.to_bytes())
        
        data.extend(self.camera.projection.to_bytes())
        
        data.extend(self.camera.view_t.to_bytes())
        
        data.extend(self.camera.position.to_bytes())

        self.buffer.write(data)

class LightUBO:
    def __init__(self, ctx: mgl.Context, camera: OrthographicCamera):
        self.ctx = ctx
        
        self.buffer_size = 128
        self.buffer = ctx.buffer(reserve=self.buffer_size)
        self.buffer.bind_to_uniform_block(binding=1)
        
        # Initialize with default lights
        self.light_color = np.asarray([0.4, 0.4, 0.4, 1.0], np.float32)
        self.camera = camera

    def update_buffer(self):
        data = bytearray()
        
        pos = Vec4(self.camera.position, 1)
    
        data.extend(pos.to_bytes())

        data.extend(self.light_color.tobytes())

        data.extend(self.camera.light_space.to_bytes())

        self.buffer.write(data)

# Used for debugging
def get_frustum_corners(view_projection_matrix: Mat44) -> list[Vec3]:
    inv_vp = view_projection_matrix.inversed()

    ndc_corners = [
        # Near plane (Z=-1)
        # Left bottom, right bottom
        Vec4(-1, -1, -1, 1), Vec4(1, -1, -1, 1),
        # Right top, left top
        Vec4(1,  1, -1, 1), Vec4(-1,  1, -1, 1),
        # Far plane (Z=1)
        Vec4(-1, -1, 1, 1), Vec4(1, -1, 1, 1),
        Vec4(1,  1, 1, 1), Vec4(-1,  1, 1, 1)
    ]

    world_corners = []
    for corner in ndc_corners:
        # Transform the corner from NDC back to world space
        transformed_corner = inv_vp * corner

        # Orthographic projection, w == 1
        # assert transformed_corner.w == 1

        # if transformed_corner.w == 0:
        #     continue

        world_corner = Vec3(
            transformed_corner.x,
            transformed_corner.y,
            transformed_corner.z
        )    

        # Prespective division
        # world_corner = math.Vec3(
        #     transformed_corner.x / transformed_corner.w,
        #     transformed_corner.y / transformed_corner.w,
        #     transformed_corner.z / transformed_corner.w
        # )

        world_corners.append(world_corner)

    return world_corners

