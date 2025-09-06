from imgui_bundle import imgui, imgui_ctx
import pyjolt

class Settings:
    def __init__(self):
        self.draw_get_triangles = False                                   # Draw all shapes using Shape::GetTrianglesStart/Next
        self.draw_constraints = False                                     # If the constraints should be drawn
        self.draw_constraint_limits = False                               # If the constraint limits should be drawn
        self.draw_constraint_reference_frame = False                      # If the constraint reference frames should be drawn
        self.draw_physics_system_bounds = False                           # If the bounds of the physics system should be drawn
        
        self.body_draw_settings = pyjolt.BodyManager.DrawSettings()               # Settings for how to draw bodies from the body manager
        self.pose_draw_settings = pyjolt.SkeletonPose.DrawSettings()              # Settings for drawing skeletal poses

        self.body_draw_settings.draw_shape = True
        self.body_draw_settings.draw_bounding_box = True

        self.contact_point = False
        self.supporting_faces = False
        self.contact_reduction = False
        self.contact_manifolds = False
        self.motion_quality = False

        self.convex_outlines = False
        self.mesh_groups = False
        self.mesh_outlines = False
        self.heightfield_outlines = False
        self.submerged_volumes = False

        self.char_constraints = False
        self.char_walk_stairs = False
        self.char_stick_floor = False

        self.show_menu = False

    def show_drawing_options_menu(self):
        if self.show_menu:
            with imgui_ctx.begin("Drawing Options", True) as custom_window:
                self.show_menu = custom_window.opened
                if custom_window.expanded:
                    # Draw Shapes (H)
                    changed, self.body_draw_settings.draw_shape = imgui.checkbox(
                        "Draw Shapes (H)", self.body_draw_settings.draw_shape
                    )
                    
                    # Draw Shapes Wireframe (Alt+W)
                    changed, self.body_draw_settings.draw_shape_wireframe = imgui.checkbox(
                        "Draw Shapes Wireframe (Alt+W)", self.body_draw_settings.draw_shape_wireframe
                    )
                    
                    # Draw Shape Color ComboBox
                    shape_color_options = ["Instance", "Shape Type", "Motion Type", "Sleep", "Island", "Material"]
                    changed, selected_color = imgui.combo(
                        "Draw Shape Color", 
                        self.body_draw_settings.draw_shape_color.value, 
                        shape_color_options
                    )
                    if changed:
                        self.body_draw_settings.draw_shape_color = selected_color
                    
                    # Draw GetSupport + Cvx Radius (Shift+H)
                    changed, self.body_draw_settings.draw_get_support_function = imgui.checkbox(
                        "Draw GetSupport + Cvx Radius (Shift+H)", self.body_draw_settings.draw_get_support_function
                    )
                    
                    # Draw Shapes Using GetTrianglesStart/Next (Alt+H)
                    changed, self.draw_get_triangles = imgui.checkbox(
                        "Draw Shapes Using GetTrianglesStart/Next (Alt+H)", self.draw_get_triangles
                    )
                    
                    # Draw GetSupport Direction
                    changed, self.body_draw_settings.draw_support_direction = imgui.checkbox(
                        "Draw GetSupport Direction", self.body_draw_settings.draw_support_direction
                    )
                    if changed and self.body_draw_settings.draw_support_direction:
                        self.body_draw_settings.draw_get_support_function = True
                    
                    # Draw GetSupportingFace (Shift+F)
                    changed, self.body_draw_settings.draw_get_supporting_face = imgui.checkbox(
                        "Draw GetSupportingFace (Shift+F)", self.body_draw_settings.draw_get_supporting_face
                    )
                    
                    # Draw Constraints (C)
                    changed, self.draw_constraints = imgui.checkbox(
                        "Draw Constraints (C)", self.draw_constraints
                    )
                    
                    # Draw Constraint Limits (L)
                    changed, self.draw_constraint_limits = imgui.checkbox(
                        "Draw Constraint Limits (L)", self.draw_constraint_limits
                    )
                    
                    # Draw Constraint Reference Frame
                    changed, self.draw_constraint_reference_frame = imgui.checkbox(
                        "Draw Constraint Reference Frame", self.draw_constraint_reference_frame
                    )
                    
                    # Contact Point and Constraint Manager options
                    changed, self.contact_point = imgui.checkbox("Draw Contact Point (1)", False)  # Replace with actual static var
                    changed, self.supporting_faces = imgui.checkbox("Draw Supporting Faces (2)", False)  # Replace with actual static var
                    changed, self.contact_reduction = imgui.checkbox("Draw Contact Point Reduction (3)", False)  # Replace with actual static var
                    changed, self.contact_manifolds = imgui.checkbox("Draw Contact Manifolds (M)", False)  # Replace with actual static var
                    changed, self.motion_quality = imgui.checkbox("Draw Motion Quality Linear Cast", False)  # Replace with actual static var
                    
                    # Draw Bounding Boxes
                    changed, self.body_draw_settings.draw_bounding_box = imgui.checkbox(
                        "Draw Bounding Boxes", self.body_draw_settings.draw_bounding_box
                    )
                    
                    # Draw Physics System Bounds
                    changed, self.draw_physics_system_bounds = imgui.checkbox(
                        "Draw Physics System Bounds", self.draw_physics_system_bounds
                    )
                    
                    # Draw Center of Mass Transforms
                    changed, self.body_draw_settings.draw_center_of_mass_transform = imgui.checkbox(
                        "Draw Center of Mass Transforms", self.body_draw_settings.draw_center_of_mass_transform
                    )
                    
                    # Draw World Transforms
                    changed, self.body_draw_settings.draw_world_transform = imgui.checkbox(
                        "Draw World Transforms", self.body_draw_settings.draw_world_transform
                    )
                    
                    # Draw Velocity
                    changed, self.body_draw_settings.draw_velocity = imgui.checkbox(
                        "Draw Velocity", self.body_draw_settings.draw_velocity
                    )
                    
                    # Draw Sleep Stats
                    changed, self.body_draw_settings.draw_sleep_stats = imgui.checkbox(
                        "Draw Sleep Stats", self.body_draw_settings.draw_sleep_stats
                    )
                    
                    # Draw Mass and Inertia (I)
                    changed, self.body_draw_settings.draw_mass_and_inertia = imgui.checkbox(
                        "Draw Mass and Inertia (I)", self.body_draw_settings.draw_mass_and_inertia
                    )
                    
                    # Draw Joints
                    changed, self.pose_draw_settings.draw_joints = imgui.checkbox(
                        "Draw Joints", self.pose_draw_settings.draw_joints
                    )
                    
                    # Draw Joint Orientations
                    changed, self.pose_draw_settings.draw_joint_orientations = imgui.checkbox(
                        "Draw Joint Orientations", self.pose_draw_settings.draw_joint_orientations
                    )
                    
                    # Draw Joint Names
                    changed, self.pose_draw_settings.draw_joint_names = imgui.checkbox(
                        "Draw Joint Names", self.pose_draw_settings.draw_joint_names
                    )
                    
                    changed, self.convex_outlines = imgui.checkbox("Draw Convex Hull Shape Face Outlines", False)
                    changed, self.mesh_groups = imgui.checkbox("Draw Mesh Shape Triangle Groups", False)
                    changed, self.mesh_outlines = imgui.checkbox("Draw Mesh Shape Triangle Outlines", False)
                    changed, self.heightfield_outlines = imgui.checkbox("Draw Height Field Shape Triangle Outlines", False)
                    changed, self.submerged_volumes = imgui.checkbox("Draw Submerged Volumes", False)
                    
                    # Character Virtual options
                    changed, self.char_constraints = imgui.checkbox("Draw Character Virtual Constraints", False)
                    changed, self.char_walk_stairs = imgui.checkbox("Draw Character Virtual Walk Stairs", False)
                    changed, self.char_stick_floor = imgui.checkbox("Draw Character Virtual Stick To Floor", False)
                    
                    # Soft Body drawing options
                    changed, self.body_draw_settings.draw_soft_body_vertices = imgui.checkbox(
                        "Draw Soft Body Vertices", self.body_draw_settings.draw_soft_body_vertices
                    )
                    
                    changed, self.body_draw_settings.draw_soft_body_vertex_velocities = imgui.checkbox(
                        "Draw Soft Body Vertex Velocities", self.body_draw_settings.draw_soft_body_vertex_velocities
                    )
                    
                    changed, self.body_draw_settings.draw_soft_body_edge_constraints = imgui.checkbox(
                        "Draw Soft Body Edge Constraints", self.body_draw_settings.draw_soft_body_edge_constraints
                    )
                    
                    changed, self.body_draw_settings.draw_soft_body_bend_constraints = imgui.checkbox(
                        "Draw Soft Body Bend Constraints", self.body_draw_settings.draw_soft_body_bend_constraints
                    )
                    
                    changed, self.body_draw_settings.draw_soft_body_volume_constraints = imgui.checkbox(
                        "Draw Soft Body Volume Constraints", self.body_draw_settings.draw_soft_body_volume_constraints
                    )
                    
                    changed, self.body_draw_settings.draw_soft_body_skin_constraints = imgui.checkbox(
                        "Draw Soft Body Skin Constraints", self.body_draw_settings.draw_soft_body_skin_constraints
                    )
                    
                    changed, self.body_draw_settings.draw_soft_body_lra_constraints = imgui.checkbox(
                        "Draw Soft Body LRA Constraints", self.body_draw_settings.draw_soft_body_lra_constraints
                    )
                    
                    changed, self.body_draw_settings.draw_soft_body_predicted_bounds = imgui.checkbox(
                        "Draw Soft Body Predicted Bounds", self.body_draw_settings.draw_soft_body_predicted_bounds
                    )
                    
                    # Draw Soft Body Constraint Color ComboBox
                    soft_body_color_options = ["Constraint Type", "Constraint Group", "Constraint Order"]
                    changed, selected_soft_color = imgui.combo(
                        "Draw Soft Body Constraint Color",
                        self.body_draw_settings.draw_soft_body_constraint_color.value,
                        soft_body_color_options
                    )
                    if changed:
                        self.body_draw_settings.draw_soft_body_constraint_color = selected_soft_color
