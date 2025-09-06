cmake_minimum_required(VERSION 3.15...3.26)

project(pyjolt LANGUAGES CXX)

if (NOT SKBUILD)
#   message(WARNING "/
#   This CMake file is meant to be executed using 'scikit-build'. Running
#   it directly will almost certainly not produce the desired result. If
#   you are a user trying to install this package, please use the command
#   below, which will install all necessary build dependencies, compile
#   the package in an isolated environment, and then install it.
#   =====================================================================
#    $ pip install .
#   =====================================================================
#   If you are a software developer, and this is your own package, then
#   it is usually much more efficient to install the build dependencies
#   in your environment once and use the following command that avoids
#   a costly creation of a new virtual environment at every compilation:
#   =====================================================================
#    $ pip install nanobind scikit-build-core[pyproject]
#    $ pip install --no-build-isolation -ve .
#   =====================================================================
#   You may optionally add -Ceditable.rebuild=true to auto-rebuild when
#   the package is imported. Otherwise, you need to re-run the above
#   after editing C++ files.")
endif()

# Try to import all Python components potentially needed by nanobind
find_package(Python 3.8
  REQUIRED COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

# Import nanobind through CMake's find_package mechanism
find_package(nanobind CONFIG REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

set(PHYSICS_REPO_ROOT "${CMAKE_SOURCE_DIR}/JoltPhysics/Jolt")
set(OBJECT_LAYER_BITS 16)

set(USE_SSE4_1 ON)                          # Enable SSE
set(USE_SSE4_2 ON)
set(USE_AVX ON)
set(USE_AVX2 ON)
set(USE_F16C ON)
set(USE_LZCNT ON)
set(USE_TZCNT ON)
set(USE_FMADD ON)

set(INTERPROCEDURAL_OPTIMIZATION ON)        # Enable interprocedural optimizations
set(ENABLE_OBJECT_STREAM ON)                # Enable Object stream
set(DEBUG_RENDERER_IN_DEBUG_AND_RELEASE ON) # Enable debug renderer
set(PROFILER_IN_DISTRIBUTION ON)            # Enable profiler
set(OVERRIDE_CXX_FLAGS ON)                  # Enable C++ flags

set(USE_AVX512 OFF)                         # Disable AVX512
set(CPP_EXCEPTIONS_ENABLED OFF)             # Disable C++ exceptions
set(CROSS_PLATFORM_DETERMINISTIC OFF)       # Disable cross platform determinisim
set(USE_STD_VECTOR OFF)                     # Disable the use of C++ std libs
set(TRACK_BROADPHASE_STATS OFF)             # Disable broadphase stats
set(TRACK_NARROWPHASE_STATS OFF)            # Disable narrowphase stats
set(DOUBLE_PRECISION OFF)                   # Disable double precision
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF)  # Disable fp exception
set(DISABLE_CUSTOM_ALLOCATOR ON)            # Disable Allocator

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CPP_RTTI_ENABLED ON)  # Enable C++ RTTI for debug builds
else()
    set(CPP_RTTI_ENABLED OFF)
endif()

# Add interface lib
include(${CMAKE_SOURCE_DIR}/cmake/Jolt.cmake)

nanobind_add_module(    
    pyjolt_ext
    STABLE_ABI
    NB_STATIC

    ${JOLT_PHYSICS_SRC_FILES}

	src/BindingUtility/Frustum.cpp
	src/BindingUtility/ArrayWrapper.cpp
	src/BindingUtility/Perlin.cpp
	JoltPhysics/TestFramework/Math/Perlin.cpp

    # Root
    src/main.cpp
	src/RegisterTypes.cpp
    src/ConfigurationString.cpp

    # AABBTree
    src/AABBTree/NodeCodec/NodeCodecQuadTreeHalfFloat.cpp
    src/AABBTree/TriangleCodec/TriangleCodecIndexed8BitPackSOA4Flags.cpp
    src/AABBTree/AABBTreeBuilder.cpp
    src/AABBTree/AABBTreeToBuffer.cpp

    # Core
	src/Core/ARMNeon.cpp
	src/Core/Array.cpp
	src/Core/Atomics.cpp
	src/Core/ByteBuffer.cpp
	src/Core/Color.cpp
	src/Core/Core.cpp
	src/Core/Factory.cpp
	src/Core/FixedSizeFreeList.cpp
	src/Core/FPControlWord.cpp
	src/Core/FPException.cpp
	src/Core/FPFlushDenormals.cpp
	src/Core/HashCombine.cpp
	src/Core/InsertionSort.cpp
	src/Core/IssueReporting.cpp
	src/Core/JobSystem.cpp
	src/Core/JobSystemSingleThreaded.cpp
	src/Core/JobSystemThreadPool.cpp
	src/Core/JobSystemWithBarrier.cpp
	src/Core/LinearCurve.cpp
	src/Core/LockFreeHashMap.cpp
	src/Core/Memory.cpp
	src/Core/Mutex.cpp
	src/Core/MutexArray.cpp
	src/Core/NonCopyable.cpp
	src/Core/Profiler.cpp
	src/Core/QuickSort.cpp
	src/Core/Reference.cpp
	src/Core/Result.cpp
	src/Core/RTTI.cpp
	src/Core/ScopeExit.cpp
	src/Core/Semaphore.cpp
	src/Core/StaticArray.cpp
	src/Core/STLAlignedAllocator.cpp
	src/Core/STLAllocator.cpp
	src/Core/STLTempAllocator.cpp
	src/Core/StreamIn.cpp
	src/Core/StreamOut.cpp
	src/Core/StreamUtils.cpp
	src/Core/StreamWrapper.cpp
	src/Core/StridedPtr.cpp
	src/Core/StringTools.cpp
	src/Core/TempAllocator.cpp
	src/Core/TickCounter.cpp
	src/Core/UnorderedMap.cpp
	src/Core/UnorderedSet.cpp

    # Geometry
    src/Geometry/AABox.cpp
    src/Geometry/AABox4.cpp
    src/Geometry/ClipPoly.cpp
    src/Geometry/ClosestPoint.cpp
    src/Geometry/ConvexHullBuilder.cpp
    src/Geometry/ConvexHullBuilder2D.cpp
    src/Geometry/ConvexSupport.cpp
    src/Geometry/Ellipse.cpp
    src/Geometry/EPAConvexHullBuilder.cpp
    src/Geometry/EPAPenetrationDepth.cpp
    src/Geometry/GJKClosestPoint.cpp
    src/Geometry/IndexedTriangle.cpp
	src/Geometry/Indexify.cpp
	src/Geometry/MortonCode.cpp
	src/Geometry/OrientedBox.cpp
	src/Geometry/Plane.cpp
    src/Geometry/RayAABox.cpp
	src/Geometry/RayCapsule.cpp
	src/Geometry/RayCylinder.cpp
	src/Geometry/RaySphere.cpp
	src/Geometry/RayTriangle.cpp
	src/Geometry/Sphere.cpp
	src/Geometry/Triangle.cpp

    # Math
	src/Math/DMat44.cpp
	src/Math/Double3.cpp
	src/Math/DVec3.cpp
	src/Math/DynMatrix.cpp
	src/Math/EigenValueSymmetric.cpp
	src/Math/FindRoot.cpp
	src/Math/Float2.cpp
	src/Math/Float3.cpp
	src/Math/Float4.cpp
	src/Math/GaussianElimination.cpp
	src/Math/HalfFloat.cpp
	src/Math/Mat44.cpp
	src/Math/Math.cpp
	src/Math/MathTypes.cpp
	src/Math/Matrix.cpp
	src/Math/Quat.cpp
	src/Math/Real.cpp
	src/Math/Swizzle.cpp
	src/Math/Trigonometry.cpp
	src/Math/UVec4.cpp
	src/Math/Vec3.cpp
	src/Math/Vec4.cpp
	src/Math/Vector.cpp
    
    # Object stream
	src/ObjectStream/GetPrimitiveTypeOfType.cpp
	src/ObjectStream/ObjectStream.cpp
	src/ObjectStream/ObjectStreamBinaryIn.cpp
	src/ObjectStream/ObjectStreamBinaryOut.cpp
	src/ObjectStream/ObjectStreamIn.cpp
	src/ObjectStream/ObjectStreamOut.cpp
	src/ObjectStream/ObjectStreamTextIn.cpp
	src/ObjectStream/ObjectStreamTextOut.cpp
	src/ObjectStream/ObjectStreamTypes.cpp
	src/ObjectStream/SerializableAttribute.cpp
	src/ObjectStream/SerializableAttributeEnum.cpp
	src/ObjectStream/SerializableAttributeTyped.cpp
	src/ObjectStream/SerializableObject.cpp
	src/ObjectStream/TypeDeclarations.cpp

    # Physics
    src/Physics/DeterminismLog.cpp
    src/Physics/EActivation.cpp
    src/Physics/EPhysicsUpdateError.cpp
    src/Physics/IslandBuilder.cpp
    src/Physics/LargeIslandSplitter.cpp
    src/Physics/PhysicsLock.cpp
    src/Physics/PhysicsScene.cpp
    src/Physics/PhysicsSettings.cpp
    src/Physics/PhysicsStepListener.cpp
    src/Physics/PhysicsSystem.cpp
    src/Physics/PhysicsUpdateContext.cpp
    src/Physics/StateRecorder.cpp
    src/Physics/StateRecorderImpl.cpp

    # Body
    src/Physics/Body/AllowedDOFs.cpp
    src/Physics/Body/Body.cpp
    src/Physics/Body/BodyAccess.cpp
    src/Physics/Body/BodyActivationListener.cpp
    src/Physics/Body/BodyCreationSettings.cpp
    src/Physics/Body/BodyFilter.cpp
    src/Physics/Body/BodyID.cpp
    src/Physics/Body/BodyInterface.cpp
    src/Physics/Body/BodyLock.cpp
    src/Physics/Body/BodyLockInterface.cpp
    src/Physics/Body/BodyLockMulti.cpp
    src/Physics/Body/BodyManager.cpp
    src/Physics/Body/BodyPair.cpp
    src/Physics/Body/BodyType.cpp
    src/Physics/Body/MassProperties.cpp
    src/Physics/Body/MotionProperties.cpp
    src/Physics/Body/MotionQuality.cpp
    src/Physics/Body/MotionType.cpp

    # Character
    src/Physics/Character/Character.cpp
    src/Physics/Character/CharacterBase.cpp
    src/Physics/Character/CharacterVirtual.cpp

    # Collision
    src/Physics/Collision/AABoxCast.cpp
    src/Physics/Collision/ActiveEdgeMode.cpp
    src/Physics/Collision/ActiveEdges.cpp
    src/Physics/Collision/BackFaceMode.cpp
    src/Physics/Collision/CastConvexVsTriangles.cpp
    src/Physics/Collision/CastResult.cpp
    src/Physics/Collision/CastSphereVsTriangles.cpp
    src/Physics/Collision/CollectFacesMode.cpp
    src/Physics/Collision/CollideConvexVsTriangles.cpp
    src/Physics/Collision/CollidePointResult.cpp
    src/Physics/Collision/CollideShape.cpp
    src/Physics/Collision/CollideSoftBodyVertexIterator.cpp
    src/Physics/Collision/CollideSoftBodyVerticesVsTriangles.cpp
    src/Physics/Collision/CollideSphereVsTriangles.cpp
    src/Physics/Collision/CollisionCollector.cpp
    src/Physics/Collision/CollisionCollectorImpl.cpp
    src/Physics/Collision/CollisionDispatch.cpp
    src/Physics/Collision/CollisionGroup.cpp
    src/Physics/Collision/ContactListener.cpp
    src/Physics/Collision/EstimateCollisionResponse.cpp
    src/Physics/Collision/GroupFilter.cpp
    src/Physics/Collision/GroupFilterTable.cpp
    src/Physics/Collision/InternalEdgeRemovingCollector.cpp
    src/Physics/Collision/ManifoldBetweenTwoFaces.cpp
    src/Physics/Collision/NarrowPhaseQuery.cpp
    src/Physics/Collision/NarrowPhaseStats.cpp
    src/Physics/Collision/ObjectLayer.cpp
    src/Physics/Collision/ObjectLayerPairFilterMask.cpp
    src/Physics/Collision/ObjectLayerPairFilterTable.cpp
    src/Physics/Collision/PhysicsMaterial.cpp
    src/Physics/Collision/PhysicsMaterialSimple.cpp
    src/Physics/Collision/RayCast.cpp
    src/Physics/Collision/ShapeCast.cpp
    src/Physics/Collision/ShapeFilter.cpp
    src/Physics/Collision/SortReverseAndStore.cpp
    src/Physics/Collision/TransformedShape.cpp

    # BroadPhase
    src/Physics/Collision/BroadPhase/BroadPhase.cpp
    src/Physics/Collision/BroadPhase/BroadPhaseBruteForce.cpp
    src/Physics/Collision/BroadPhase/BroadPhaseLayer.cpp
    src/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.cpp
    src/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.cpp
    src/Physics/Collision/BroadPhase/BroadPhaseQuadTree.cpp
    src/Physics/Collision/BroadPhase/BroadPhaseQuery.cpp
    src/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.cpp
    src/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.cpp
    src/Physics/Collision/BroadPhase/QuadTree.cpp

    # Shape
    src/Physics/Collision/Shape/BoxShape.cpp
    src/Physics/Collision/Shape/CapsuleShape.cpp
    src/Physics/Collision/Shape/CompoundShape.cpp
    src/Physics/Collision/Shape/CompoundShapeVisitors.cpp
    src/Physics/Collision/Shape/ConvexHullShape.cpp
    src/Physics/Collision/Shape/ConvexShape.cpp
    src/Physics/Collision/Shape/CylinderShape.cpp
    src/Physics/Collision/Shape/DecoratedShape.cpp
    src/Physics/Collision/Shape/EmptyShape.cpp
    src/Physics/Collision/Shape/GetTrianglesContext.cpp
    src/Physics/Collision/Shape/HeightFieldShape.cpp
    src/Physics/Collision/Shape/MeshShape.cpp
    src/Physics/Collision/Shape/MutableCompoundShape.cpp
    src/Physics/Collision/Shape/OffsetCenterOfMassShape.cpp
    src/Physics/Collision/Shape/PlaneShape.cpp
    src/Physics/Collision/Shape/PolyhedronSubmergedVolumeCalculator.cpp
    src/Physics/Collision/Shape/RotatedTranslatedShape.cpp
    src/Physics/Collision/Shape/ScaledShape.cpp
    src/Physics/Collision/Shape/ScaleHelpers.cpp
    src/Physics/Collision/Shape/Shape.cpp
    src/Physics/Collision/Shape/SphereShape.cpp
    src/Physics/Collision/Shape/StaticCompoundShape.cpp
    src/Physics/Collision/Shape/SubShapeID.cpp
    src/Physics/Collision/Shape/SubShapeIDPair.cpp
    src/Physics/Collision/Shape/TaperedCapsuleShape.cpp
    src/Physics/Collision/Shape/TaperedCylinderShape.cpp
    src/Physics/Collision/Shape/TriangleShape.cpp

    # Constraints
    src/Physics/Constraints/CalculateSolverSteps.cpp
    src/Physics/Constraints/ConeConstraint.cpp
    src/Physics/Constraints/Constraint.cpp
    src/Physics/Constraints/ConstraintManager.cpp
    src/Physics/Constraints/ContactConstraintManager.cpp
    src/Physics/Constraints/DistanceConstraint.cpp
    src/Physics/Constraints/FixedConstraint.cpp
    src/Physics/Constraints/GearConstraint.cpp
    src/Physics/Constraints/HingeConstraint.cpp
    src/Physics/Constraints/MotorSettings.cpp
    src/Physics/Constraints/PathConstraint.cpp
    src/Physics/Constraints/PathConstraintPath.cpp
    src/Physics/Constraints/PathConstraintPathHermite.cpp
    src/Physics/Constraints/PointConstraint.cpp
    src/Physics/Constraints/PulleyConstraint.cpp
    src/Physics/Constraints/RackAndPinionConstraint.cpp
    src/Physics/Constraints/SixDOFConstraint.cpp
    src/Physics/Constraints/SliderConstraint.cpp
    src/Physics/Constraints/SpringSettings.cpp
    src/Physics/Constraints/SwingTwistConstraint.cpp
    src/Physics/Constraints/TwoBodyConstraint.cpp

    # ConstraintPart
    src/Physics/Constraints/ConstraintPart/AngleConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/AxisConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/DualAxisConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/GearConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/HingeRotationConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/IndependentAxisConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/PointConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/RackAndPinionConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/RotationEulerConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/RotationQuatConstraintPart.cpp
    src/Physics/Constraints/ConstraintPart/SpringPart.cpp
    src/Physics/Constraints/ConstraintPart/SwingTwistConstraintPart.cpp

    # Ragdoll
    src/Physics/Ragdoll/Ragdoll.cpp

    # SoftBody
    src/Physics/SoftBody/SoftBodyContactListener.cpp
    src/Physics/SoftBody/SoftBodyCreationSettings.cpp
    src/Physics/SoftBody/SoftBodyManifold.cpp
    src/Physics/SoftBody/SoftBodyMotionProperties.cpp
    src/Physics/SoftBody/SoftBodyShape.cpp
    src/Physics/SoftBody/SoftBodySharedSettings.cpp
    src/Physics/SoftBody/SoftBodyUpdateContext.cpp
    src/Physics/SoftBody/SoftBodyVertex.cpp

    # Vehicle
    src/Physics/Vehicle/MotorcycleController.cpp
    src/Physics/Vehicle/TrackedVehicleController.cpp
    src/Physics/Vehicle/VehicleAntiRollBar.cpp
    src/Physics/Vehicle/VehicleCollisionTester.cpp
    src/Physics/Vehicle/VehicleConstraint.cpp
    src/Physics/Vehicle/VehicleController.cpp
    src/Physics/Vehicle/VehicleDifferential.cpp
    src/Physics/Vehicle/VehicleEngine.cpp
    src/Physics/Vehicle/VehicleTrack.cpp
    src/Physics/Vehicle/VehicleTransmission.cpp
    src/Physics/Vehicle/Wheel.cpp
    src/Physics/Vehicle/WheeledVehicleController.cpp

	src/Renderer/DebugRenderer.cpp
	src/Renderer/DebugRendererPlayback.cpp
	src/Renderer/DebugRendererRecorder.cpp
	src/Renderer/DebugRendererSimple.cpp

	src/Skeleton/SkeletalAnimation.cpp
	src/Skeleton/Skeleton.cpp
	src/Skeleton/SkeletonMapper.cpp
	src/Skeleton/SkeletonPose.cpp

    src/TriangleGrouper/TriangleGrouper.cpp
	src/TriangleGrouper/TriangleGrouperClosestCentroid.cpp
	src/TriangleGrouper/TriangleGrouperMorton.cpp

	src/TriangleSplitter/TriangleSplitter.cpp
	src/TriangleSplitter/TriangleSplitterBinning.cpp
	src/TriangleSplitter/TriangleSplitterFixedLeafSize.cpp
	src/TriangleSplitter/TriangleSplitterLongestAxis.cpp
	src/TriangleSplitter/TriangleSplitterMean.cpp
	src/TriangleSplitter/TriangleSplitterMorton.cpp
)

target_compile_definitions(pyjolt_ext PRIVATE JPH_PYTHON_BINDINGS)

# Set include
target_include_directories(pyjolt_ext PRIVATE src/)
target_include_directories(pyjolt_ext PRIVATE JoltPhysics)
target_include_directories(pyjolt_ext PRIVATE JoltPhysics/TestFramework)

# Link Jolt
target_link_libraries(pyjolt_ext PRIVATE Jolt)

set(PYTHON_SOURCE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/python")

nanobind_add_stub(
    pyjolt_ext_stub
    MODULE pyjolt_ext
    PYTHON_PATH $<TARGET_FILE_DIR:pyjolt_ext>
    DEPENDS pyjolt_ext

    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pyjolt_ext.pyi
)
nanobind_add_stub(
    pyjolt_math_stub
    MODULE pyjolt_ext.math
    PYTHON_PATH $<TARGET_FILE_DIR:pyjolt_ext>
    DEPENDS pyjolt_ext

    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/__init__.pyi
)

# Install the .pyd file
install(TARGETS pyjolt_ext
    LIBRARY DESTINATION pyjolt
)

# Install __init__.py file
install(DIRECTORY "${PYTHON_SOURCE_ROOT_DIR}/"
    DESTINATION pyjolt
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    PATTERN "*/__pycache__" EXCLUDE
)

# Install the generated stubs
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pyjolt_ext.pyi"
    DESTINATION pyjolt
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/__init__.pyi"
    DESTINATION pyjolt/math
)
