#include "Common.h"

#define BIND(x, module) void x(nb::module_ &m); x(module)

NB_MODULE(pyjolt_ext, mainModule) {
    // Forward Bind
    BIND(BindNonCopyable, mainModule);
    BIND(BindReference, mainModule);

    // Main module
    BIND(BindConfigurationString, mainModule);
    BIND(BindRegisterTypes, mainModule);

    BIND(BindFrustum, mainModule);
    BIND(BindPerlin, mainModule);

    // Bind math
    auto math_m = mainModule.def_submodule("math", "Math functions");
    BIND(BindDMat44, math_m);
    BIND(BindDouble3, math_m);
    BIND(BindDVec3, math_m);
    BIND(BindDynMatrix, math_m);
    BIND(BindEigenValueSymmetric, math_m);
    BIND(BindFindRoot, math_m);
    BIND(BindFloat2, math_m);
    BIND(BindFloat3, math_m);
    BIND(BindFloat4, math_m);
    BIND(BindGaussianElimination, math_m);
    BIND(BindHalfFloat, math_m);
    BIND(BindMat44, math_m);
    BIND(BindMath, math_m);
    BIND(BindMathTypes, math_m);
    BIND(BindMatrix, math_m);
    BIND(BindQuat, math_m);
    BIND(BindReal, math_m);
    BIND(BindSwizzle, math_m);
    BIND(BindTrigonometry, math_m);
    BIND(BindUVec4, math_m);
    BIND(BindVec3, math_m);
    BIND(BindVec4, math_m);
    BIND(BindVector, math_m);

    // Bind AABBTree
    BIND(BindNodeCodecQuadTreeHalfFloat, mainModule);
    BIND(BindTriangleCodecIndexed8BitPackSOA4Flags, mainModule);
    BIND(BindAABBTreeBuilder, mainModule);
    BIND(BindAABBTreeToBuffer, mainModule);

    // Bind Core
    BIND(BindARMNeon, mainModule);
    BIND(BindArray, mainModule);
    BIND(BindAtomics, mainModule);
    BIND(BindByteBuffer, mainModule);
    BIND(BindColor, mainModule);
    BIND(BindCore, mainModule);
    BIND(BindFactory, mainModule);
    BIND(BindFixedSizeFreeList, mainModule);
    BIND(BindFPControlWord, mainModule);
    BIND(BindFPException, mainModule);
    BIND(BindFPFlushDenormals, mainModule);
    BIND(BindHashCombine, mainModule);
    BIND(BindInsertionSort, mainModule);
    BIND(BindIssueReporting, mainModule);
    BIND(BindJobSystem, mainModule);
    BIND(BindJobSystemWithBarrier, mainModule);
    BIND(BindJobSystemSingleThreaded, mainModule);
    BIND(BindJobSystemThreadPool, mainModule);
    BIND(BindLinearCurve, mainModule);
    BIND(BindLockFreeHashMap, mainModule);
    BIND(BindMemory, mainModule);
    BIND(BindMutex, mainModule);
    BIND(BindMutexArray, mainModule);
    BIND(BindProfiler, mainModule);
    BIND(BindQuickSort, mainModule);
    BIND(BindResult, mainModule);
    BIND(BindRTTI, mainModule);
    BIND(BindScopeExit, mainModule);
    BIND(BindSemaphore, mainModule);
    BIND(BindStaticArray, mainModule);
    BIND(BindSTLAlignedAllocator, mainModule);
    BIND(BindSTLAllocator, mainModule);
    BIND(BindSTLTempAllocator, mainModule);
    BIND(BindStreamIn, mainModule);
    BIND(BindStreamOut, mainModule);
    BIND(BindStreamUtils, mainModule);
    BIND(BindStreamWrapper, mainModule);
    BIND(BindStridedPtr, mainModule);
    BIND(BindStringTools, mainModule);
    BIND(BindTempAllocator, mainModule);
    BIND(BindTickCounter, mainModule);
    BIND(BindUnorderedMap, mainModule);
    BIND(BindUnorderedSet, mainModule);

    // Bind Geometry
    BIND(BindAABox, mainModule);
    BIND(BindAABox4, mainModule);
    BIND(BindClipPoly, mainModule);
    BIND(BindClosestPoint, mainModule);
    BIND(BindConvexHullBuilder, mainModule);
    BIND(BindConvexHullBuilder2D, mainModule);
    BIND(BindConvexSupport, mainModule);
    BIND(BindEllipse, mainModule);
    BIND(BindEPAConvexHullBuilder, mainModule);
    BIND(BindEPAPenetrationDepth, mainModule);
    BIND(BindGJKClosestPoint, mainModule);
    BIND(BindIndexedTriangle, mainModule);
    BIND(BindIndexify, mainModule);
    BIND(BindMortonCode, mainModule);
    BIND(BindOrientedBox, mainModule);
    BIND(BindPlane, mainModule);
    BIND(BindRayAABox, mainModule);
    BIND(BindRayCapsule, mainModule);
    BIND(BindRayCylinder, mainModule);
    BIND(BindRaySphere, mainModule);
    BIND(BindRayTriangle, mainModule);
    BIND(BindSphere, mainModule);
    BIND(BindTriangle, mainModule);

    // ObjectStream
    BIND(BindGetPrimitiveTypeOfType, mainModule);
    BIND(BindObjectStream, mainModule);
    BIND(BindObjectStreamIn, mainModule);
    BIND(BindObjectStreamOut, mainModule);
    BIND(BindObjectStreamBinaryIn, mainModule);
    BIND(BindObjectStreamBinaryOut, mainModule);
    BIND(BindObjectStreamTextIn, mainModule);
    BIND(BindObjectStreamTextOut, mainModule);
    BIND(BindObjectStreamTypes, mainModule);
    BIND(BindSerializableAttribute, mainModule);
    BIND(BindSerializableAttributeEnum, mainModule);
    BIND(BindSerializableAttributeTyped, mainModule);
    BIND(BindSerializableObject, mainModule);
    BIND(BindTypeDeclarations, mainModule);

    // Physics
    BIND(BindDeterminismLog, mainModule);
    BIND(BindEActivation, mainModule);
    BIND(BindEPhysicsUpdateError, mainModule);
    BIND(BindIslandBuilder, mainModule);
    BIND(BindLargeIslandSplitter, mainModule);
    BIND(BindPhysicsLock, mainModule);
    BIND(BindPhysicsScene, mainModule);
    BIND(BindPhysicsSettings, mainModule);
    BIND(BindPhysicsStepListener, mainModule);
    BIND(BindPhysicsSystem, mainModule);
    BIND(BindPhysicsUpdateContext, mainModule);
    BIND(BindStateRecorder, mainModule);
    BIND(BindStateRecorderImpl, mainModule);

    // Body
    BIND(BindAllowedDOFs, mainModule);
    BIND(BindBody, mainModule);
    BIND(BindBodyAccess, mainModule);
    BIND(BindBodyActivationListener, mainModule);
    BIND(BindBodyCreationSettings, mainModule);
    BIND(BindBodyFilter, mainModule);
    BIND(BindBodyID, mainModule);
    BIND(BindBodyInterface, mainModule);
    BIND(BindBodyLock, mainModule);
    BIND(BindBodyLockInterface, mainModule);
    BIND(BindBodyLockMulti, mainModule);
    BIND(BindBodyManager, mainModule);
    BIND(BindBodyPair, mainModule);
    BIND(BindBodyType, mainModule);
    BIND(BindMassProperties, mainModule);
    BIND(BindMotionProperties, mainModule);
    BIND(BindMotionQuality, mainModule);
    BIND(BindMotionType, mainModule);

    // Character
    BIND(BindCharacterBase, mainModule);
    BIND(BindCharacterVirtual, mainModule);
    BIND(BindCharacter, mainModule);

    // Collision
    BIND(BindAABoxCast, mainModule);
    BIND(BindActiveEdgeMode, mainModule);
    BIND(BindActiveEdges, mainModule);
    BIND(BindBackFaceMode, mainModule);
    BIND(BindCastConvexVsTriangles, mainModule);
    BIND(BindCastResult, mainModule);
    BIND(BindCastSphereVsTriangles, mainModule);
    BIND(BindCollectFacesMode, mainModule);
    BIND(BindCollideConvexVsTriangles, mainModule);
    BIND(BindCollidePointResult, mainModule);
    BIND(BindCollideShape, mainModule);
    BIND(BindCollideSoftBodyVertexIterator, mainModule);
    BIND(BindCollideSoftBodyVerticesVsTriangles, mainModule);
    BIND(BindCollideSphereVsTriangles, mainModule);
    BIND(BindCollisionCollector, mainModule);
    BIND(BindCollisionCollectorImpl, mainModule);
    BIND(BindCollisionDispatch, mainModule);
    BIND(BindCollisionGroup, mainModule);
    BIND(BindContactListener, mainModule);
    BIND(BindEstimateCollisionResponse, mainModule);
    BIND(BindGroupFilter, mainModule);
    BIND(BindGroupFilterTable, mainModule);
    BIND(BindInternalEdgeRemovingCollector, mainModule);
    BIND(BindManifoldBetweenTwoFaces, mainModule);
    BIND(BindNarrowPhaseQuery, mainModule);
    BIND(BindNarrowPhaseStats, mainModule);
    BIND(BindObjectLayer, mainModule);
    BIND(BindObjectLayerPairFilterMask, mainModule);
    BIND(BindObjectLayerPairFilterTable, mainModule);
    BIND(BindPhysicsMaterial, mainModule);
    BIND(BindPhysicsMaterialSimple, mainModule);
    BIND(BindRayCast, mainModule);
    BIND(BindShapeCast, mainModule);
    BIND(BindShapeFilter, mainModule);
    BIND(BindSortReverseAndStore, mainModule);
    BIND(BindTransformedShape, mainModule);

    // BroadPhase
    BIND(BindBroadPhaseQuery, mainModule);
    BIND(BindBroadPhase, mainModule);
    BIND(BindBroadPhaseBruteForce, mainModule);
    BIND(BindBroadPhaseLayer, mainModule);
    BIND(BindBroadPhaseLayerInterfaceMask, mainModule);
    BIND(BindBroadPhaseLayerInterfaceTable, mainModule);
    BIND(BindBroadPhaseQuadTree, mainModule);
    BIND(BindObjectVsBroadPhaseLayerFilterMask, mainModule);
    BIND(BindObjectVsBroadPhaseLayerFilterTable, mainModule);
    BIND(BindQuadTree, mainModule);

    // Shape
    BIND(BindShape, mainModule);
    BIND(BindConvexShape, mainModule);
    BIND(BindBoxShape, mainModule);
    BIND(BindCapsuleShape, mainModule);
    BIND(BindCompoundShape, mainModule);
    BIND(BindCompoundShapeVisitors, mainModule);
    BIND(BindConvexHullShape, mainModule);
    BIND(BindCylinderShape, mainModule);
    BIND(BindDecoratedShape, mainModule);
    BIND(BindEmptyShape, mainModule);
    BIND(BindGetTrianglesContext, mainModule);
    BIND(BindHeightFieldShape, mainModule);
    BIND(BindMeshShape, mainModule);
    BIND(BindMutableCompoundShape, mainModule);
    BIND(BindOffsetCenterOfMassShape, mainModule);
    BIND(BindPlaneShape, mainModule);
    BIND(BindPolyhedronSubmergedVolumeCalculator, mainModule);
    BIND(BindRotatedTranslatedShape, mainModule);
    BIND(BindScaledShape, mainModule);
    BIND(BindScaleHelpers, mainModule);
    BIND(BindSphereShape, mainModule);
    BIND(BindStaticCompoundShape, mainModule);
    BIND(BindSubShapeID, mainModule);
    BIND(BindSubShapeIDPair, mainModule);
    BIND(BindTaperedCapsuleShape, mainModule);
    BIND(BindTaperedCylinderShape, mainModule);
    BIND(BindTriangleShape, mainModule);

    // Constraints
    BIND(BindConstraint, mainModule);
    BIND(BindTwoBodyConstraint, mainModule);
    BIND(BindCalculateSolverSteps, mainModule);
    BIND(BindConeConstraint, mainModule);
    BIND(BindConstraintManager, mainModule);
    BIND(BindContactConstraintManager, mainModule);
    BIND(BindDistanceConstraint, mainModule);
    BIND(BindFixedConstraint, mainModule);
    BIND(BindGearConstraint, mainModule);
    BIND(BindHingeConstraint, mainModule);
    BIND(BindMotorSettings, mainModule);
    BIND(BindPathConstraint, mainModule);
    BIND(BindPathConstraintPath, mainModule);
    BIND(BindPathConstraintPathHermite, mainModule);
    BIND(BindPointConstraint, mainModule);
    BIND(BindPulleyConstraint, mainModule);
    BIND(BindRackAndPinionConstraint, mainModule);
    BIND(BindSixDOFConstraint, mainModule);
    BIND(BindSliderConstraint, mainModule);
    BIND(BindSpringSettings, mainModule);
    BIND(BindSwingTwistConstraint, mainModule);

    // ConstraintPart
    BIND(BindAngleConstraintPart, mainModule);
    BIND(BindAxisConstraintPart, mainModule);
    BIND(BindDualAxisConstraintPart, mainModule);
    BIND(BindGearConstraintPart, mainModule);
    BIND(BindHingeRotationConstraintPart, mainModule);
    BIND(BindIndependentAxisConstraintPart, mainModule);
    BIND(BindPointConstraintPart, mainModule);
    BIND(BindRackAndPinionConstraintPart, mainModule);
    BIND(BindRotationEulerConstraintPart, mainModule);
    BIND(BindRotationQuatConstraintPart, mainModule);
    BIND(BindSpringPart, mainModule);
    BIND(BindSwingTwistConstraintPart, mainModule);

    // Ragdoll
    BIND(BindRagdoll, mainModule);

    // SoftBody
    BIND(BindSoftBodyContactListener, mainModule);
    BIND(BindSoftBodyCreationSettings, mainModule);
    BIND(BindSoftBodyManifold, mainModule);
    BIND(BindSoftBodyShape, mainModule);
    BIND(BindSoftBodySharedSettings, mainModule);
    BIND(BindSoftBodyUpdateContext, mainModule);
    BIND(BindSoftBodyVertex, mainModule);
    BIND(BindSoftBodyMotionProperties, mainModule);

    // Vehicle
    BIND(BindVehicleController, mainModule);
    BIND(BindWheel, mainModule);
    BIND(BindWheeledVehicleController, mainModule);
    BIND(BindMotorcycleController, mainModule);
    BIND(BindTrackedVehicleController, mainModule);
    BIND(BindVehicleAntiRollBar, mainModule);
    BIND(BindVehicleCollisionTester, mainModule);
    BIND(BindVehicleConstraint, mainModule);
    BIND(BindVehicleDifferential, mainModule);
    BIND(BindVehicleEngine, mainModule);
    BIND(BindVehicleTrack, mainModule);
    BIND(BindVehicleTransmission, mainModule);

    // Renderer
    BIND(BindDebugRenderer, mainModule);
    BIND(BindDebugRendererPlayback, mainModule);
    BIND(BindDebugRendererRecorder, mainModule);
    BIND(BindDebugRendererSimple, mainModule);

    // Skeleton
    BIND(BindSkeletalAnimation, mainModule);
    BIND(BindSkeleton, mainModule);
    BIND(BindSkeletonMapper, mainModule);
    BIND(BindSkeletonPose, mainModule);

    // TriangleGrouper
    BIND(BindTriangleGrouper, mainModule);
    BIND(BindTriangleGrouperClosestCentroid, mainModule);
    BIND(BindTriangleGrouperMorton, mainModule);

    // TriangleSplitter
    BIND(BindTriangleSplitter, mainModule);
    BIND(BindTriangleSplitterBinning, mainModule);
    BIND(BindTriangleSplitterFixedLeafSize, mainModule);
    BIND(BindTriangleSplitterLongestAxis, mainModule);
    BIND(BindTriangleSplitterMean, mainModule);
    BIND(BindTriangleSplitterMorton, mainModule);
}