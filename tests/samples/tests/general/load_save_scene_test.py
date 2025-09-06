import pyjolt
from pyjolt import (
    Color, PhysicsScene, BodyCreationSettings, ScaledShapeSettings,
    TriangleShapeSettings, SphereShapeSettings, BoxShapeSettings, CapsuleShapeSettings,
    TaperedCapsuleShapeSettings, CylinderShapeSettings, TaperedCylinderShapeSettings,
    EmptyShapeSettings, StaticCompoundShapeSettings, ConvexHullShapeSettings,
    RotatedTranslatedShapeSettings, MutableCompoundShapeSettings, DistanceConstraintSettings,
    SoftBodyCreationSettings, SoftBodySharedSettings, PhysicsMaterialSimple, MeshShapeSettings, Triangle
)
from pyjolt.math import Vec3, Quat, Float3
from pyjolt import math
from layers import Layers


class LoadSaveSceneTest:
    c_max_height = 10.0

    @staticmethod
    def create_mesh() -> MeshShapeSettings:
        n = 10
        cell_size = 2.0

        # Create heights
        heights = [[0.0] * (n + 1) for _ in range(n + 1)]
        for x in range(n + 1):
            for z in range(n + 1):
                # Assuming pyjolt.perlin_noise3 and c_max_height are available
                heights[x][z] = LoadSaveSceneTest.c_max_height * pyjolt.perlin_noise3(
                    float(x) / n, 0.0, float(z) / n, 256, 256, 256)

        # Create 'wall' around grid
        for x in range(n + 1):
            heights[x][0] += 2.0
            heights[x][n] += 2.0
        
        # In C++, the loop variable 'y' was used for the z-axis
        for z in range(1, n):
            heights[0][z] += 2.0
            heights[n][z] += 2.0

        # Create regular grid of triangles
        max_material_index = 0
        triangles = pyjolt.TriangleArray()
        for x in range(n):
            for z in range(n):
                center = n * cell_size / 2.0

                x1 = cell_size * x - center
                z1 = cell_size * z - center
                x2 = x1 + cell_size
                z2 = z1 + cell_size

                v1 = Float3(x1, heights[x][z], z1)
                v2 = Float3(x2, heights[x + 1][z], z1)
                v3 = Float3(x1, heights[x][z + 1], z2)
                v4 = Float3(x2, heights[x + 1][z + 1], z2)

                material_index = int((Vec3(v1) + Vec3(v2) + Vec3(v3) + Vec3(v4)).length() / 4.0 / cell_size)
                max_material_index = max(max_material_index, material_index)
                
                triangles.push_back(Triangle(v1, v3, v4, material_index))
                triangles.push_back(Triangle(v1, v4, v2, material_index))

        # Create materials
        materials = pyjolt.PhysicsMaterialListArray()
        for i in range(max_material_index + 1):
            # TODO:
            # material = PhysicsMaterialSimple(f"Mesh Material {i}", Color.get_distinct_color(i))
            material = pyjolt.PhysicsMaterialRefConst()
            materials.push_back(material)
        
        return MeshShapeSettings(triangles, materials)

    @staticmethod
    def create_height_field() -> pyjolt.HeightFieldShapeSettings:
        n = 32
        cell_size = 1.0

        # Create height samples
        heights = [0.0] * (n * n)
        for y in range(n):
            for x in range(n):
                heights[y * n + x] = LoadSaveSceneTest.c_max_height * pyjolt.perlin_noise3(
                    float(x) / n, 0.0, float(y) / n, 256, 256, 256)

        # Make a hole
        heights[2 * n + 2] = pyjolt.NO_COLLISION_VALUE

        # Make material indices
        max_material_index = 0
        num_materials = (n - 1) * (n - 1)
        material_indices = [0] * num_materials
        center_offset = Vec3(n * cell_size / 2.0, 0, n * cell_size / 2.0)
        
        for y in range(n - 1):
            for x in range(n - 1):
                pos = Vec3(x * cell_size, 0, y * cell_size)
                material_index = int(round((pos - center_offset).length() / 10.0))
                max_material_index = max(max_material_index, material_index)
                material_indices[y * (n - 1) + x] = material_index
        
        # Create materials
        materials = pyjolt.PhysicsMaterialListArray()
        for i in range(max_material_index + 1):
            # material = PhysicsMaterialSimple(f"HeightField Material {i}", Color.s_get_distinct_color(i))
            material = pyjolt.PhysicsMaterialRefConst()
            materials.push_back(material)

        # Create height field
        offset = Vec3(-0.5 * cell_size * n, 0.0, -0.5 * cell_size * n)
        scale = Vec3(cell_size, 1.0, cell_size)
        
        return pyjolt.HeightFieldShapeSettings(heights, offset, scale, n, material_indices, materials)

    @staticmethod
    def create_scene() -> PhysicsScene:
        color = 0
        def next_color():
            nonlocal color
            c = Color.get_distinct_color(color)
            color += 1
            return c

        pos = Vec3(0, LoadSaveSceneTest.c_max_height, 0)
        def next_pos():
            nonlocal pos
            pos += Vec3(0, 1.0, 0)
            return pos

        # Create scene
        scene = PhysicsScene()

        # A scaled mesh floor
        scene.add_body(BodyCreationSettings(
            ScaledShapeSettings(LoadSaveSceneTest.create_mesh(), Vec3(2.5, 1.0, 1.5)),
            Vec3(0, 0, 0), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING))

        # A heightfield floor
        scene.add_body(BodyCreationSettings(
            LoadSaveSceneTest.create_height_field(),
            Vec3(50, 0, 0), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING))

        # Some simple primitives
        scene.add_body(BodyCreationSettings(TriangleShapeSettings(Vec3(-2, 0, 0), Vec3(0, 1, 0), Vec3(2, 0, 0), 0.0, PhysicsMaterialSimple("Triangle Material", next_color())), next_pos(), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), pyjolt.EMotionType.STATIC, Layers.NON_MOVING))
        scene.add_body(BodyCreationSettings(SphereShapeSettings(0.2, PhysicsMaterialSimple("Sphere Material", next_color())), next_pos(), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))
        scene.add_body(BodyCreationSettings(BoxShapeSettings(Vec3(0.2, 0.2, 0.4), 0.01, PhysicsMaterialSimple("Box Material", next_color())), next_pos(), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))
        scene.add_body(BodyCreationSettings(CapsuleShapeSettings(1.5, 0.2, PhysicsMaterialSimple("Capsule Material", next_color())), next_pos(), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))
        scene.add_body(BodyCreationSettings(TaperedCapsuleShapeSettings(0.5, 0.1, 0.2, PhysicsMaterialSimple("Tapered Capsule Material", next_color())), next_pos(), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))
        scene.add_body(BodyCreationSettings(CylinderShapeSettings(0.5, 0.2, pyjolt.DEFAULT_CONVEX_RADIUS, PhysicsMaterialSimple("Cylinder Material", next_color())), next_pos(), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))
        scene.add_body(BodyCreationSettings(TaperedCylinderShapeSettings(0.5, 0.2, 0.4, pyjolt.DEFAULT_CONVEX_RADIUS, PhysicsMaterialSimple("Tapered Cylinder Material", next_color())), next_pos(), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))
        scene.add_body(BodyCreationSettings(TaperedCylinderShapeSettings(0.5, 0.4, 0.0, 0.0, PhysicsMaterialSimple("Cone Material", next_color())), next_pos(), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))
        scene.add_body(BodyCreationSettings(EmptyShapeSettings(), next_pos(), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))

        # Compound with sub compound and rotation
        sub_compound = StaticCompoundShapeSettings()
        sub_compound.add_shape(Vec3(0, 0.5, 0), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), BoxShapeSettings(Vec3(0.5, 0.1, 0.2), pyjolt.DEFAULT_CONVEX_RADIUS, PhysicsMaterialSimple("Compound Box Material", next_color())))
        sub_compound.add_shape(Vec3(0.5, 0, 0), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), CylinderShapeSettings(0.5, 0.2, pyjolt.DEFAULT_CONVEX_RADIUS, PhysicsMaterialSimple("Compound Cylinder Material", next_color())))
        sub_compound.add_shape(Vec3(0, 0, 0.5), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), TaperedCapsuleShapeSettings(0.5, 0.1, 0.2, PhysicsMaterialSimple("Compound Tapered Capsule Material", next_color())))
        
        compound_shape = StaticCompoundShapeSettings()
        compound_shape.add_shape(Vec3(0, 0, 0), Quat.rotation(Vec3.axis_x(), -0.25 * math.JPH_PI) * Quat.rotation(Vec3.axis_z(), 0.25 * math.JPH_PI), sub_compound)
        compound_shape.add_shape(Vec3(0, -0.1, 0), Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI) * Quat.rotation(Vec3.axis_z(), -0.75 * math.JPH_PI), sub_compound)
        scene.add_body(BodyCreationSettings(compound_shape, next_pos(), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))

        # Convex hull shape
        tetrahedron = pyjolt.Vec3Array()
        tetrahedron.push_back(Vec3(-0.5, 0, -0.5))
        tetrahedron.push_back(Vec3(0, 0, 0.5))
        tetrahedron.push_back(Vec3(0.5, 0, -0.5))
        tetrahedron.push_back(Vec3(0, -0.5, 0))
        convex_hull = ConvexHullShapeSettings(tetrahedron, pyjolt.DEFAULT_CONVEX_RADIUS, PhysicsMaterialSimple("Convex Hull Material", next_color()))
        scene.add_body(BodyCreationSettings(convex_hull, next_pos(), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))

        # Rotated convex hull
        scene.add_body(BodyCreationSettings(RotatedTranslatedShapeSettings(Vec3.replicate(0.5), Quat.rotation(Vec3.axis_z(), 0.25 * math.JPH_PI), convex_hull), next_pos(), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))

        # Mutable compound
        mutable_compound = MutableCompoundShapeSettings()
        mutable_compound.add_shape(Vec3(0, 0.5, 0), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), BoxShapeSettings(Vec3(0.5, 0.1, 0.2), pyjolt.DEFAULT_CONVEX_RADIUS, PhysicsMaterialSimple("MutableCompound Box Material", next_color())))
        mutable_compound.add_shape(Vec3(0.5, 0, 0), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), CapsuleShapeSettings(0.5, 0.1, PhysicsMaterialSimple("MutableCompound Capsule Material", next_color())))
        mutable_compound.add_shape(Vec3(0, 0, 0.5), Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI), TaperedCapsuleShapeSettings(0.5, 0.2, 0.1, PhysicsMaterialSimple("MutableCompound Tapered Capsule Material", next_color())))
        scene.add_body(BodyCreationSettings(mutable_compound, next_pos(), Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING))

        # Connect the first two dynamic bodies with a distance constraint (Sphere and Box)
        # Body indices are: 0=Mesh(S), 1=HeightField(S), 2=Triangle(S), 3=Sphere(D), 4=Box(D)
        dist_constraint = DistanceConstraintSettings()
        dist_constraint.space = pyjolt.EConstraintSpace.LOCAL_TO_BODY_COM
        scene.add_constraint(dist_constraint, 3, 4)

        # Add soft body cube
        # sb_cube_settings = SoftBodyCreator.create_cube(5, 0.2)
        # sb_cube_settings.materials = [PhysicsMaterialSimple("Soft Body Cube Material", next_color())]
        # sb_cube = SoftBodyCreationSettings(sb_cube_settings, next_pos(), Quat.identity(), Layers.MOVING)
        # scene.add_soft_body(sb_cube)

        # Add the same shape again to test sharing
        # sb_cube.position = next_pos()
        # scene.add_soft_body(sb_cube)

        # Add soft body sphere
        # sb_sphere_settings = SoftBodyCreator.create_sphere(0.5)
        # sb_sphere_settings.materials = [PhysicsMaterialSimple("Soft Body Sphere Material", next_color())]
        # sb_sphere = SoftBodyCreationSettings(sb_sphere_settings, next_pos(), Quat.identity(), Layers.MOVING)
        # sb_sphere.pressure = 2000.0
        # scene.add_soft_body(sb_sphere)

        return scene