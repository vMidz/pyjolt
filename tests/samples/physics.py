import pyjolt
from pyjolt import Color
from pyjolt.math import Vec3, Mat44
from enum import IntEnum
import numpy as np
import moderngl as mgl
from dataclasses import dataclass, field
from camera import Camera, OrthographicCamera
from shaders_utility import load_shader, Text
from layers import BPLayerInterfaceImpl, ObjectVsBroadPhaseLayerFilterImpl, ObjectLayerPairFilterImpl
from tests.test_common import pre_physics_update, post_physics_update
from settings import Settings
from imgui_bundle import imgui
from shaders_utility import get_frustum_corners

# Constants
NUM_BODIES = 10_240
NUM_BODY_MUTEXES = 0 # auto detect
MAX_BODY_PAIRS = 65_536
MAX_CONTACT_CONSTRAINTS = 20_480
THREAD_COUNT = 1
COLLISION_STEPS = 1

BUFFER_GROWTH_FACTOR = 1.5
SSBO_BINDING_POINT = 4
SHADOW_MAP_SIZE = 1024 * 2
SHADOW_MAP_BINDING_POINT = 3

CLEAR_COLOR = (0.2, 0.2, 0.3, 1.0)

class Pass(IntEnum):
    DEPTH = 0
    GEOM = 1

debug_object_shader_buffer_dtype = np.dtype([
    ('model', 'f4', (4, 4)),
    ('inverseNormal', 'f4', (4, 4)),
    ('color', 'u4'),
    ('padding', 'V12')
], align=True)

draw_elements_indirect_command_dtype = np.dtype([
    ('count', 'u4'),
    ('instanceCount', 'u4'),
    ('firstIndex', 'u4'),
    ('baseVertex', 'u4'),
    ('baseInstance', 'u4'),
])

debug_vertex_dtype_from_flat = np.dtype([
    ('position', 'f4', (3,1)),
    ('normal',   'f4', (3,1)),
    ('uv',       'f4', (2,1)),
    ('color',    'f4', (1,1)) 
])

def assert_func(inExpression: str, inMessage: str, inFile: str, inLine: int) -> None:
    print(inExpression, inMessage, inFile, inLine)
    return True         # True == exit, Flase continue

def trace_func(msg: str) -> None:
    print('Trace:', msg)

@dataclass
class DebugLine:
    position: Vec3 = field(default_factory=Vec3)
    color: Color = field(default_factory=Color)

@dataclass
class DebugMesh:
    vertices: np.ndarray = field(default_factory=lambda: np.array([], dtype=np.float32))
    indices: np.ndarray = field(default_factory=lambda: np.array([], dtype=np.uint32))

class RenderPass:
    def __init__(self, ctx: mgl.Context):
        self.gpu_command_buffer = ctx.buffer(reserve=len(draw_elements_indirect_command_dtype), dynamic=True)
        self.pending_draw: list[int] = []
        self.issued_draw: list[int] = []
        self.ssbo_bytes = bytearray()
        self.ssbo_buffer = ctx.buffer(reserve=debug_object_shader_buffer_dtype.itemsize , dynamic=True)

    def append_object(self, index: int, ssbo_data: bytearray):
        self.pending_draw.append(index)
        self.ssbo_bytes.extend(ssbo_data)

    def clear_pending_objects(self):
        self.pending_draw.clear()
        self.ssbo_bytes.clear()

class DebugRendererImpl(pyjolt.DebugRenderer):
    LINES_LAYOUT = ('3f 4f1', 'aPos', 'aColor')
    MESH_LAYOUT = ('3f 3f 2f 1i', 'aPos', 'aNormal', 'aUv', 'aColor')
    UNINDEXED_LAYOUT = ('3f 4f1', 'in_position', 'in_color')

    def __init__(self, physics_system: pyjolt.PhysicsSystem, ctx: mgl.Context, main_camera: Camera, shadow_map_cam: OrthographicCamera, draw_settings: Settings):
        LINES_INITIAL_SIZE = 1024
        GEOM_INITIAL_SIZE = 4096

        self.ctx = ctx
        self.main_camera = main_camera
        self.shadow_map_cam = shadow_map_cam
        self.physics_system = physics_system
        self.settings = draw_settings

        self.pending_text: list[Text] = []

        # Unindexed triangles
        VERTEX_SHADER = """
        #version 460 core
        
        in vec3 in_position;
        in vec4 in_color;
        uniform mat4 mvp;
        out vec4 vertex_color;
        
        void main() {
            gl_Position = mvp * vec4(in_position, 1.0);
            vertex_color = in_color;
        }
        """
        
        FRAGMENT_SHADER = """
        #version 460 core
        
        in vec4 vertex_color;
        out vec4 fragColor;
        
        void main() {
            fragColor = vertex_color;
        }
        """
        self.unindexed_triangles_shader = self.ctx.program(
            vertex_shader=VERTEX_SHADER,
            fragment_shader=FRAGMENT_SHADER
        )
        self.unindexed_triangles = bytearray()

        # lines rendering
        self.lines_vbo = ctx.buffer(reserve=16 * LINES_INITIAL_SIZE, dynamic=True)

        self.lines_shader = load_shader(ctx,"lines.vert", "lines.frag")
        self.lines_vao = ctx.vertex_array(
            self.lines_shader,
            [(self.lines_vbo, *self.LINES_LAYOUT)]
        )
        self.lines_data = bytearray()

        # Triangles rendering
        self.geom_vbo = ctx.buffer(reserve=36 * GEOM_INITIAL_SIZE, dynamic=True)
        self.geom_vao_layout = [
            (self.geom_vbo, *self.MESH_LAYOUT)
        ]

        self.depth_shader = load_shader(ctx, 'shadow_map.vert', 'shadow_map.frag')
        self.geom_shader = load_shader(ctx, "main.vert", "main.frag")
        self.geom_ebo = ctx.buffer(reserve=4 * GEOM_INITIAL_SIZE, dynamic=True)
        self.geom_vao = ctx.vertex_array(
            self.geom_shader,
            self.geom_vao_layout,
            index_buffer=self.geom_ebo,
            index_element_size=4 
        )
        self.depth_vao = ctx.vertex_array(
            self.depth_shader,
            self.geom_vao_layout,
            index_buffer=self.geom_ebo,
            index_element_size=4 
        )

        # SSBO Binding
        self.geom_shader['SSBOBuffer'].binding = SSBO_BINDING_POINT
        self.depth_shader['SSBOBuffer'].binding = SSBO_BINDING_POINT

        self.meshes: list[DebugMesh] = []
        self.render_passes=[RenderPass(ctx) ,RenderPass(ctx)]

        self.mesh_index_offsets: list[int] = []
        self.mesh_vertex_offsets: list[int] = []

        self.gpu_buffer_updated = False

        # Shadow map
        self.u_shadow_map = SHADOW_MAP_BINDING_POINT
        self.depth_texture = self.ctx.depth_texture((SHADOW_MAP_SIZE, SHADOW_MAP_SIZE))
        self.depth_texture.use(SHADOW_MAP_BINDING_POINT)
        sampler=self.ctx.sampler(False, False, False, None, 1.0, '<=', (1.0,1.0,1.0,1.0))
        sampler.use(SHADOW_MAP_BINDING_POINT)
        self.depth_texture.repeat_x = False
        self.depth_texture.repeat_y = False
        self.depth_fbo = self.ctx.framebuffer(color_attachments=[], depth_attachment=self.depth_texture)

        super().__init__()

    def draw_lines(self):
        if not self.lines_data:
            return

        required_size = len(self.lines_data)

        if self.lines_vbo.size < required_size:
            self.lines_vbo.release()
            self.lines_vao.release()
            self.lines_vbo = self.ctx.buffer(reserve=int(required_size * BUFFER_GROWTH_FACTOR))
            
            self.lines_vao = self.ctx.vertex_array(
                self.lines_shader,
                [(self.lines_vbo, *self.LINES_LAYOUT)]
            )

        self.lines_vbo.write(self.lines_data, offset=0)

        num_vertices = required_size // 16 # 16 bytes per vertex
        self.lines_vao.render(mgl.LINES, vertices=num_vertices)
        
        self.lines_data = bytearray()

    def update_commands(self, render_pass_index: int):
        commands_list = []
        for mesh_index in self.render_passes[render_pass_index].pending_draw:
            mesh = self.meshes[mesh_index]
            cmd = (
                len(mesh.indices),                        # count
                1,                                        # instanceCount
                self.mesh_index_offsets[mesh_index],      # firstIndex
                self.mesh_vertex_offsets[mesh_index],     # baseVertex
                0                                         # baseInstance
            )
            commands_list.append(cmd)
        
        commands_np = np.array(commands_list, dtype=draw_elements_indirect_command_dtype)
        required_cmd_size = commands_np.nbytes
        
        if self.render_passes[render_pass_index].gpu_command_buffer.size < required_cmd_size:
            self.render_passes[render_pass_index].gpu_command_buffer.release()
            self.render_passes[render_pass_index].gpu_command_buffer = self.ctx.buffer(reserve=int(required_cmd_size * BUFFER_GROWTH_FACTOR))

        if required_cmd_size > 0:
            self.render_passes[render_pass_index].gpu_command_buffer.write(commands_np.tobytes())
        else:
            assert 0 # Not possible

        self.render_passes[render_pass_index].issued_draw = self.render_passes[render_pass_index].pending_draw.copy()
        
    def update_gpu_buffer(self):
        if not self.meshes:
            return

        self.mesh_vertex_offsets.clear()
        self.mesh_index_offsets.clear()
        total_vertices = 0
        total_indices = 0

        for mesh in self.meshes:
            self.mesh_vertex_offsets.append(total_vertices)
            self.mesh_index_offsets.append(total_indices)

            total_vertices += mesh.vertices.shape[0]
            total_indices += mesh.indices.shape[0]

        # Combine all data into single numpy arrays
        all_vertices = np.concatenate([mesh.vertices for mesh in self.meshes])
        all_indices = np.concatenate([mesh.indices for mesh in self.meshes])
 
        required_vbo_size = all_vertices.nbytes
        required_ebo_size = all_indices.nbytes
            
        self.geom_vbo = self.ctx.buffer(reserve=int(required_vbo_size * BUFFER_GROWTH_FACTOR))
        self.geom_ebo = self.ctx.buffer(reserve=int(required_ebo_size * BUFFER_GROWTH_FACTOR))

        self._rebuild_vaos()

        if required_vbo_size > 0:
            self.geom_vbo.write(all_vertices.tobytes())
        
        if required_ebo_size > 0:
            self.geom_ebo.write(all_indices.tobytes())
        
        self.gpu_buffer_updated = True

    def _rebuild_vaos(self) -> None:
        """Rebuild vertex array objects after buffer resize"""
        self.geom_vao.release()
        self.depth_vao.release()
        
        self.geom_vao_layout[0] = (self.geom_vbo, *self.MESH_LAYOUT)
        
        self.geom_vao = self.ctx.vertex_array(
            self.geom_shader, self.geom_vao_layout,
            index_buffer=self.geom_ebo, index_element_size=4
        )
        self.depth_vao = self.ctx.vertex_array(
            self.depth_shader, self.geom_vao_layout,
            index_buffer=self.geom_ebo, index_element_size=4
        )

    def write_ssbo_buffer(self, buffer: mgl.Buffer, data: bytes) -> mgl.Buffer:
        required_size = len(data)
        if buffer.size < required_size:
            # print(f"Debug: Resizing buffer from {buffer.size} to {required_size} bytes.")
            buffer.release()
            buffer = self.ctx.buffer(reserve=int(required_size * BUFFER_GROWTH_FACTOR))

        buffer.write(data)
        return buffer

    def draw_scene(self):
        geom_pass = self.render_passes[Pass.GEOM]
        depth_pass = self.render_passes[Pass.DEPTH]

        # Nothing to draw
        if not geom_pass.pending_draw:
            self.ctx.screen.clear(*CLEAR_COLOR)
            depth_pass.clear_pending_objects()
            geom_pass.clear_pending_objects()
            return

        if not self.gpu_buffer_updated:
            self.update_gpu_buffer()

        # draw depth
        depth_pass.ssbo_buffer = self.write_ssbo_buffer(depth_pass.ssbo_buffer, depth_pass.ssbo_bytes)
        depth_pass.ssbo_buffer.bind_to_storage_buffer(binding=SSBO_BINDING_POINT)
        if depth_pass.issued_draw != depth_pass.pending_draw:
            self.update_commands(Pass.DEPTH)
        self.depth_fbo.use()
        self.depth_fbo.viewport=(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE)
        self.depth_fbo.clear()
        if len(depth_pass.issued_draw) > 0:
             self.depth_vao.render_indirect(depth_pass.gpu_command_buffer, count=len(depth_pass.issued_draw))

        # Geometry
        geom_pass.ssbo_buffer = self.write_ssbo_buffer(geom_pass.ssbo_buffer, geom_pass.ssbo_bytes)
        geom_pass.ssbo_buffer.bind_to_storage_buffer(binding=SSBO_BINDING_POINT)
        if geom_pass.issued_draw != geom_pass.pending_draw:
            self.update_commands(Pass.GEOM)
        self.ctx.screen.use()

        self.ctx.screen.clear(*CLEAR_COLOR)
        if len(geom_pass.issued_draw) > 0:
            self.geom_vao.render_indirect(geom_pass.gpu_command_buffer, count=len(geom_pass.issued_draw))

        # Reset per-frame data
        geom_pass.clear_pending_objects()
        depth_pass.clear_pending_objects()

    # The actual drawing function
    def draw(self):
        self.physics_system.draw_bodies(self.settings.body_draw_settings, self)
        self.draw_frustum(self.shadow_map_cam.light_space) # TODO: debugging
        self.draw_scene()
        self.draw_unindexed_triangles()
        self.draw_lines()

    def draw_unindexed_triangles(self):
        if not self.unindexed_triangles:
            return

        mat = self.main_camera.projection * self.main_camera.view
        self.unindexed_triangles_shader['mvp'] = mat.to_numpy().flatten()

        vbo = self.ctx.buffer(self.unindexed_triangles, dynamic=True)
        vao = self.ctx.vertex_array(
            self.unindexed_triangles_shader,
            [(vbo, *self.UNINDEXED_LAYOUT)]
        )
        vao.render()
        self.unindexed_triangles.clear()

    def draw_line(self, from_: Vec3, to: Vec3, color: Color) -> None:
        min_x = min(to.x, from_.x)
        min_y = min(to.y, from_.y)
        min_z = min(to.z, from_.z)
        max_x = max(to.x, from_.x)
        max_y = max(to.y, from_.y)
        max_z = max(to.z, from_.z)

        b_box = pyjolt.AABox(Vec3(min_x,min_y,min_z), Vec3(max_x,max_y,max_z))

        # Cull if doesn't overlap camera frustum
        if(not self.main_camera.frustum_overlaps(b_box)):
           return

        # Extract RGBA bytes
        color_uint = color.get_uint32()
        r = (color_uint >> 0) & 0xff
        g = (color_uint >> 8) & 0xff  
        b = (color_uint >> 16) & 0xff
        a = (color_uint >> 24) & 0xff
        
        self.lines_data += from_.to_bytes()
        self.lines_data += np.array([r, g, b, a], dtype='u1').tobytes()  # u1 = unsigned byte
        
        self.lines_data += to.to_bytes()
        self.lines_data += np.array([r, g, b, a], dtype='u1').tobytes()

    def draw_triangle(self, v1: Vec3, v2: Vec3, v3: Vec3, color: Color,
                      cast_shadow: pyjolt.DebugRenderer.ECastShadow = pyjolt.DebugRenderer.ECastShadow.OFF) -> None:
        color_uint = color.get_uint32()
        r = (color_uint >> 0) & 0xff
        g = (color_uint >> 8) & 0xff  
        b = (color_uint >> 16) & 0xff
        a = (color_uint >> 24) & 0xff

        self.unindexed_triangles += v1.to_bytes()
        self.unindexed_triangles += np.array([r, g, b, a], dtype='u1').tobytes() 

        self.unindexed_triangles += v2.to_bytes()
        self.unindexed_triangles += np.array([r, g, b, a], dtype='u1').tobytes() 

        self.unindexed_triangles += v3.to_bytes()
        self.unindexed_triangles += np.array([r, g, b, a], dtype='u1').tobytes() 

    def draw_text3d(self, position: Vec3, text: str, color: pyjolt.Color = pyjolt.Color.White, height: float = 0.5) -> None:
        result = self.main_camera.world_to_screen(position)

        # clipped
        if result is None:
            return
        
        screen_pos, w_coord = result
        
        imgui_clr = imgui.IM_COL32(color.r, color.g, color.b, color.a)

        self.pending_text.append(Text(text, position, imgui_clr, height, screen_pos, w_coord))

    def create_triangle_batch_triangles(self, triangles: pyjolt.DebugRendererTriangleArray) -> int:
        geometry_index = -1

        vertices, indices = pyjolt.create_index_buffer_from_triangle_array(triangles)
        if vertices.empty() or not indices.size:
            return geometry_index

        self.gpu_buffer_updated = False

        geometry_index = len(self.meshes)

        mesh = DebugMesh()

        np_array=vertices.to_numpy()
        if np_array.size > 0:
            mesh.vertices = np_array.view(debug_vertex_dtype_from_flat).squeeze()
        else:
            assert 0      # Not possible path?
            mesh.vertices = np.array([], dtype=debug_vertex_dtype_from_flat)

        mesh.indices = np.array(indices, copy=True, dtype=np.uint32)
        self.meshes.append(mesh)

        return geometry_index

    def create_triangle_batch(self, vertices: pyjolt.DebugRendererVertexArray, indices: np.ndarray) -> int:
        geometry_index = -1

        if vertices.empty() or not indices.size:
            return geometry_index

        self.gpu_buffer_updated = False

        geometry_index = len(self.meshes)

        mesh = DebugMesh()

        np_array=vertices.to_numpy()
        if np_array.size > 0:
            mesh.vertices = np_array.view(debug_vertex_dtype_from_flat).squeeze()
        else:
            assert 0
            mesh.vertices = np.array([], dtype=debug_vertex_dtype_from_flat)

        mesh.indices = np.array(indices, copy=False, dtype=np.uint32)
        self.meshes.append(mesh)

        return geometry_index
    
    # doesn't actually draw anything just pushes data
    def draw_geometry(self, model_matrix: Mat44, world_space_bounds: pyjolt.AABox,
                      lod_scale_sq: float, model_color: Color, geometry: pyjolt.DebugRendererGeometryRef,
                      cull_mode: pyjolt.DebugRenderer.ECullMode = pyjolt.DebugRenderer.ECullMode.CULL_BACK_FACE,
                      cast_shadow: pyjolt.DebugRenderer.ECastShadow = pyjolt.DebugRenderer.ECastShadow.ON,
                      draw_mode: pyjolt.DebugRenderer.EDrawMode = pyjolt.DebugRenderer.EDrawMode.SOLID) -> None:

        ssbo_bytes = bytearray()
        ssbo_bytes.extend(model_matrix.to_bytes())
        ssbo_bytes.extend(model_matrix.get_direction_preserving_matrix().to_bytes())
        ssbo_bytes.extend(model_color.get_uint32().to_bytes(4, 'little', signed=False))
        ssbo_bytes.extend(b'\x00' * 12)          # 12 Bytes padding

        index = -1
        if cast_shadow == pyjolt.DebugRenderer.ECastShadow.ON:
            light_direction = (Vec3(10, 0, 0) - self.shadow_map_cam.position).normalized()
            
            shadow_distance = self.shadow_map_cam.far - self.shadow_map_cam.near
            
            shadow_offset = light_direction * shadow_distance
            
            shadow_receiver_min = world_space_bounds.min + shadow_offset
            shadow_receiver_max = world_space_bounds.max + shadow_offset
            
            # Combine original object bounds with shadow receiver bounds
            combined_min = Vec3.min(world_space_bounds.min, shadow_receiver_min)
            combined_max = Vec3.max(world_space_bounds.max, shadow_receiver_max)
            
            # Create the combined bounding box
            shadow_volume = pyjolt.AABox(combined_min, combined_max)
            
            # Check if this combined volume (object + its shadow area) overlaps camera frustum
            if self.main_camera.frustum_overlaps(shadow_volume):
                index = geometry.get_index(self.main_camera.position, world_space_bounds, lod_scale_sq)
                if index != -1:
                    self.render_passes[Pass.DEPTH].append_object(index, ssbo_bytes)

        if not self.main_camera.frustum_overlaps(world_space_bounds):
            return
        
        # index was not known before
        if index == -1:
            index = geometry.get_index(self.main_camera.position, world_space_bounds, lod_scale_sq)

        if index == -1:
            return

        self.render_passes[Pass.GEOM].append_object(index, ssbo_bytes)

    def draw_frustum(self, frustum):
        frustum_corners = get_frustum_corners(frustum)
        if frustum_corners: # Only draw if the corners were successfully calculated
            # 2. Define the 12 lines of the cube by connecting the corner indices
            #    (0-3 are near plane, 4-7 are far plane)
            lines_indices = [
                (0, 1), (1, 2), (2, 3), (3, 0),  # Near plane quad
                (4, 5), (5, 6), (6, 7), (7, 4),  # Far plane quad
                (0, 4), (1, 5), (2, 6), (3, 7)   # Lines connecting near and far planes
            ]

            # 3. Define a bright color for the wireframe
            debug_color = Color(255,255,0,255) # Bright Yellow

            # 4. Loop through the indices and draw each line
            for start_idx, end_idx in lines_indices:
                start_point = frustum_corners[start_idx]
                end_point = frustum_corners[end_idx]
                self.draw_line(start_point, end_point, debug_color)

class Physics:
    def __init__(self, ctx: mgl.Context, main_camera: Camera, shadow_map_cam: OrthographicCamera, settings: Settings):
        self.ctx = ctx
        self.main_camera = main_camera
        self.shadow_map_cam = shadow_map_cam

        pyjolt.register_default_allocator()
        pyjolt.new_factory()
        pyjolt.register_types()

        if pyjolt.is_debug_enabled():
            pyjolt.set_trace(trace_func)
            pyjolt.set_assert(assert_func)
 
        # Create filters
        self.broadPhaseInterface = BPLayerInterfaceImpl()
        self.objectBroadPhaseLayerFilter = ObjectVsBroadPhaseLayerFilterImpl()
        self.objectLayerPairFilter = ObjectLayerPairFilterImpl()

        self.physics_system = pyjolt.PhysicsSystem()

        self.physics_system.init(NUM_BODIES, NUM_BODY_MUTEXES, MAX_BODY_PAIRS, MAX_CONTACT_CONSTRAINTS,
                                 self.broadPhaseInterface, self.objectBroadPhaseLayerFilter, self.objectLayerPairFilter)
        # Init job system
        self.job_system = pyjolt.JobSystemThreadPool(pyjolt.MAX_PHYSICS_JOBS, pyjolt.MAX_PHYSICS_BARRIERS, THREAD_COUNT)

        # Init debug renderer
        self.debug_renderer = DebugRendererImpl(self.physics_system, ctx, main_camera, shadow_map_cam, settings)


    def update_and_draw(self, delta_time):
        TEMP_ALLOCATER_MEMORY = 32 * 1024 * 1024

        temp_allocator = pyjolt.TempAllocatorImpl(TEMP_ALLOCATER_MEMORY)

        self.debug_renderer.draw()

        pre_physics_update(delta_time)

        self.physics_system.update(delta_time, COLLISION_STEPS, temp_allocator, self.job_system)

        post_physics_update(delta_time)

    def deinit(self):
        constraints = self.physics_system.get_constraints()
        for constraint in constraints:
            self.physics_system.remove_constraint(constraint.get())

        bi = self.physics_system.get_body_interface()
        bodies = self.physics_system.get_bodies()
        for body in bodies:
            bi.remove_body(body)
            bi.destroy_body(body)

        pyjolt.unregister_types()
        pyjolt.delete_factory()
        del self.debug_renderer
 