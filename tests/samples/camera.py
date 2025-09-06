import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Mat44, Float2, Vec4
from imgui_bundle import imgui
from typing import Tuple, Optional

CAMERA_POSITION = Vec3(5.0, 2.5, 10.0)
MOUSE_SPEED = 0.001

class OrthographicCamera:
    def __init__(self, position: Vec3, target: Vec3, size: int, near: float, far: float):
        self.position = position
        self.target = target

        self.right = size
        self.left = -size
        self.top = size
        self.bottom = -size

        self.near = near
        self.far = far

        self.target = target
        self.up = Vec3(0.0, 1.0, 0.0)

        self.projection = Mat44()
        self.view = Mat44()
        self.light_space = Mat44()

    def update_camera(self) -> None:
        self.projection = Mat44.orthographic(self.left, self.right, self.bottom, self.top, self.near, self.far)
        self.view = Mat44.look_at(self.position,
                                    self.target,
                                    self.up)
        self.light_space = self.projection * self.view

class Camera:
    def __init__(self,
                position: Vec3,
                aspect: float,
                fov: float,
                near: float,
                far: float,
                width: int, height: int):
        self.near = near
        self.far = far
        self.aspect = aspect
        self.position = Vec3(position) # Copy

        self.forward = Vec3(0, 0, 1)
        self.right = Vec3(1, 0 ,0)
        self.up = Vec3(0, 1, 0)
        self.view = Mat44()
        self.projection = Mat44()
        self.view_t = Mat44()    # View without translation
        self.frustum = pyjolt.Frustum()

        self.fovy = math.degrees_to_radians(fov)

        self.fovx = 2.0 * math.a_tan(self.aspect * math.tan(0.5 * self.fovy))

        self.horizontal_angle = math.degrees_to_radians(180)
        self.vertical_angle = 0.0

        self.width = width
        self.height = height

    def change_angle(self, angle: Float2) -> None:
        self.horizontal_angle -= MOUSE_SPEED * angle.x
        self.vertical_angle -= MOUSE_SPEED * angle.y

    def update_camera(self) -> None:
        # Spherical to Cartesian coordinates
        cos_vertical = math.cos(self.vertical_angle)
        sin_vertical = math.sin(self.vertical_angle)
        cos_horizontal = math.cos(self.horizontal_angle)
        sin_horizontal = math.sin(self.horizontal_angle)
        
        self.forward = Vec3(
            cos_vertical * sin_horizontal,
            sin_vertical,
            cos_vertical * cos_horizontal
        )
        
        angle_offset = self.horizontal_angle - math.JPH_PI / 2
        self.right = Vec3(
            math.sin(angle_offset),
            0,
            math.cos(angle_offset)
        )

        self.up = self.right.cross(self.forward)

        self.frustum = pyjolt.Frustum(self.position, self.forward, self.up, 
                                       self.fovx, self.fovy, self.near)

        target = self.position + self.forward
        self.view = Mat44.look_at(self.position, target, self.up)
        self.projection = Mat44.perspective(self.fovy, self.aspect, self.near, self.far)

        # View matrix without translation
        self.view_t = Mat44(self.view) # Copy matrix
        self.view_t.translation = Vec3.zero()

    def world_to_screen(self, world_pos: Vec3) -> Float2 | None:
        # Convert to Vec4 for matrix multiplication
        world_pos_h = Vec4(world_pos.x, world_pos.y, world_pos.z, 1.0)

        # Transform to clip space
        clip_pos = self.projection * self.view * world_pos_h
        
        # The w-component is the depth. If it's negative or zero,
        # the point is behind or on the camera plane and should not be drawn.
        if clip_pos.w <= 0.0:
            return None
        
        # Perform perspective divide (normalize to NDC)
        ndc_pos = clip_pos / clip_pos.w
        
        # Convert NDC [-1, 1] to screen coordinates [0, width/height]
        screen_pos = Float2()
        screen_pos.x = (ndc_pos.x * 0.5 + 0.5) * self.width
        screen_pos.y = (1.0 - (ndc_pos.y * 0.5 + 0.5)) * self.height
        
        return screen_pos, clip_pos.w

    def is_point_inside_object_bounds(self, point: Float2, object_transform: Mat44, object_radius: float) -> bool:
        position = Vec4(object_transform.translation, 1.0)
        screen_pos = self.world_to_screen(position)
        
        dx = point.x - screen_pos.x
        dy = point.y - screen_pos.y
        distance_squared = dx * dx + dy * dy
        
        return distance_squared < object_radius * object_radius

    def is_position_visible(self, screen_pos: Float2) -> bool:
        io = imgui.get_io()
        x, y = screen_pos.x, screen_pos.y
        
        return (0 <= x <= io.display_size.x and 
                0 <= y <= io.display_size.y and
                x >= 0 and y >= 0)

    def calculate_distance_scale(self, world_pos: Vec3) -> float:
        camera_pos = self.position
        
        # Calculate distance
        dx = world_pos.x - camera_pos.x
        dy = world_pos.y - camera_pos.y
        dz = world_pos.z - camera_pos.z
        distance = (dx*dx + dy*dy + dz*dz) ** 0.5
        
        min_scale = 0.5
        max_scale = 2.0
        scale_distance = 10.0
        
        scale = scale_distance / max(distance, 0.1)
        return max(min_scale, min(max_scale, scale))

    def frustum_overlaps(self, world_space_bounds: pyjolt.AABox) -> bool:
        return self.frustum.overlaps(world_space_bounds)

    def reset(self) -> None:
        self.position = Vec3(CAMERA_POSITION)
        self.horizontal_angle = math.degrees_to_radians(180)
        self.vertical_angle = 0.0
