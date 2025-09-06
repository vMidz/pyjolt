#include "Common.h"
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>
#include <nanobind/operators.h>

void BindSoftBodySharedSettings(nb::module_ &m) {
    nb::class_<SoftBodySharedSettings, RefTarget<SoftBodySharedSettings>> softBodySharedSettingsCls(m, "SoftBodySharedSettings",
        nb::intrusive_ptr<SoftBodySharedSettings>([](SoftBodySharedSettings *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "This class defines the setup of all particles and their constraints.\n"
        "It is used during the simulation and can be shared between multiple soft bodies.");
    softBodySharedSettingsCls
        .def("create_constraints", &SoftBodySharedSettings::CreateConstraints, "vertex_attributes"_a, "vertex_attributes_length"_a, "bend_type"_a = (int)SoftBodySharedSettings::EBendType::Distance, "angle_tolerance"_a = 0.13962634f,
            "Automatically create constraints based on the faces of the soft body.\n"
            "Args:\n"
            "    vertex_attributes (VertexAttributes*): A list of attributes for each vertex (1-on-1 with mVertices, note that if the list is smaller than mVertices the last element will be repeated). This defines the properties of the constraints that are created.\n"
            "    vertex_attributes_length (int): The length of inVertexAttributes.\n"
            "    bend_type (EBendType): The type of bend constraint to create.\n"
            "    angle_tolerance (float): Shear edges are created when two connected triangles form a quad (are roughly in the same plane and form a square with roughly 90 degree angles). This defines the tolerance (in radians).")
        .def("calculate_edge_lengths", &SoftBodySharedSettings::CalculateEdgeLengths,
            "Calculate the initial lengths of all springs of the edges of this soft body (if you use CreateConstraint, this is already done)")
        .def("calculate_lra_lengths", &SoftBodySharedSettings::CalculateLRALengths, "max_distance_multiplier"_a = 1.0f,
            "Calculate the max lengths for the long range attachment constraints based on Euclidean distance (if you use CreateConstraints, this is already done).\n"
            "Args:\n"
            "    max_distance_multiplier (float): Multiplier for the max distance of the LRA constraint, e.g. 1.01 means the max distance is 1% longer than the calculated distance in the rest pose.")
        .def("calculate_bend_constraint_constants", &SoftBodySharedSettings::CalculateBendConstraintConstants,
            "Calculate the constants for the bend constraints (if you use CreateConstraints, this is already done)")
        .def("calculate_volume_constraint_volumes", &SoftBodySharedSettings::CalculateVolumeConstraintVolumes,
            "Calculates the initial volume of all tetrahedra of this soft body")
        .def("calculate_skinned_constraint_normals", &SoftBodySharedSettings::CalculateSkinnedConstraintNormals,
            "Calculate information needed to be able to calculate the skinned constraint normals at run-time")
        .def("optimize", nb::overload_cast<SoftBodySharedSettings::OptimizationResults &>(&SoftBodySharedSettings::Optimize), "results"_a,
            "Optimize the soft body settings for simulation. This will reorder constraints so they can be executed in parallel.")
        .def("optimize", nb::overload_cast<>(&SoftBodySharedSettings::Optimize),
            "Optimize the soft body settings without results")
        .def("clone", &SoftBodySharedSettings::Clone,
            "Clone this object")
        .def("save_binary_state", &SoftBodySharedSettings::SaveBinaryState, "stream"_a,
            "Saves the state of this object in binary form to inStream. Doesn't store the material list.")
        .def("restore_binary_state", &SoftBodySharedSettings::RestoreBinaryState, "stream"_a,
            "Restore the state of this object from inStream. Doesn't restore the material list.")
        .def("save_with_materials", &SoftBodySharedSettings::SaveWithMaterials, "stream"_a, "settings_map"_a, "material_map"_a,
            "Save this shared settings and its materials. Pass in an empty map ioSettingsMap / ioMaterialMap or reuse the same map while saving multiple settings objects to the same stream in order to avoid writing duplicates.")
        .def_static("restore_with_materials", &SoftBodySharedSettings::sRestoreWithMaterials, "stream"_a, "settings_map"_a, "material_map"_a,
            "Restore a shape and materials. Pass in an empty map in ioSettingsMap / ioMaterialMap or reuse the same map while reading multiple settings objects from the same stream in order to restore duplicates.")
        .def("add_face", &SoftBodySharedSettings::AddFace, "face"_a,
            "Add a face to this soft body")
        .def_rw("vertices", &SoftBodySharedSettings::mVertices,
            "The list of vertices or particles of the body")
        .def_rw("faces", &SoftBodySharedSettings::mFaces,
            "The list of faces of the body")
        .def_rw("edge_constraints", &SoftBodySharedSettings::mEdgeConstraints,
            "The list of edges or springs of the body")
        .def_rw("dihedral_bend_constraints", &SoftBodySharedSettings::mDihedralBendConstraints,
            "The list of dihedral bend constraints of the body")
        .def_rw("volume_constraints", &SoftBodySharedSettings::mVolumeConstraints,
            "The list of volume constraints of the body that keep the volume of tetrahedra in the soft body constant")
        .def_rw("skinned_constraints", &SoftBodySharedSettings::mSkinnedConstraints,
            "The list of vertices that are constrained to a skinned vertex")
        .def_rw("inv_bind_matrices", &SoftBodySharedSettings::mInvBindMatrices,
            "The list of inverse bind matrices for skinning vertices")
        .def_rw("lra_constraints", &SoftBodySharedSettings::mLRAConstraints,
            "The list of long range attachment constraints")
        .def_rw("materials", &SoftBodySharedSettings::mMaterials,
            "The materials of the faces of the body, referenced by Face::mMaterialIndex")
        .def_rw("vertex_radius", &SoftBodySharedSettings::mVertexRadius,
            "How big the particles are, can be used to push the vertices a little bit away from the surface of other bodies to prevent z-fighting");

    nb::class_<SoftBodySharedSettings::VertexAttributes> vertexAttributesCls(softBodySharedSettingsCls, "VertexAttributes",
        "Per vertex attributes used during the CreateConstraints function.\n"
        "For an edge or shear constraint, the compliance is averaged between the two attached vertices.\n"
        "For a bend constraint, the compliance is averaged between the two vertices on the shared edge.");
    vertexAttributesCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<float, float, float, SoftBodySharedSettings::ELRAType, float>(), "compliance"_a, "shear_compliance"_a, "bend_compliance"_a, "lra_type"_a = (int)SoftBodySharedSettings::ELRAType::None, "lra_max_distance_multiplier"_a = 1.0f)
        .def_rw("compliance", &SoftBodySharedSettings::VertexAttributes::mCompliance,
            "The compliance of the normal edges. Set to FLT_MAX to disable regular edges for any edge involving this vertex.")
        .def_rw("shear_compliance", &SoftBodySharedSettings::VertexAttributes::mShearCompliance,
            "The compliance of the shear edges. Set to FLT_MAX to disable shear edges for any edge involving this vertex.")
        .def_rw("bend_compliance", &SoftBodySharedSettings::VertexAttributes::mBendCompliance,
            "The compliance of the bend edges. Set to FLT_MAX to disable bend edges for any bend constraint involving this vertex.")
        .def_rw("lra_type", &SoftBodySharedSettings::VertexAttributes::mLRAType,
            "The type of long range attachment constraint to create.")
        .def_rw("lra_max_distance_multiplier", &SoftBodySharedSettings::VertexAttributes::mLRAMaxDistanceMultiplier,
            "Multiplier for the max distance of the LRA constraint, e.g. 1.01 means the max distance is 1% longer than the calculated distance in the rest pose.");

    nb::class_<SoftBodySharedSettings::OptimizationResults> optimizationResultsCls(softBodySharedSettingsCls, "OptimizationResults",
        "Information about the optimization of the soft body, the indices of certain elements may have changed.");
    optimizationResultsCls
        .def_rw("edge_remap", &SoftBodySharedSettings::OptimizationResults::mEdgeRemap,
            "Maps old edge index to new edge index")
        .def_rw("lra_remap", &SoftBodySharedSettings::OptimizationResults::mLRARemap,
            "Maps old LRA index to new LRA index")
        .def_rw("dihedral_bend_remap", &SoftBodySharedSettings::OptimizationResults::mDihedralBendRemap,
            "Maps old dihedral bend index to new dihedral bend index")
        .def_rw("volume_remap", &SoftBodySharedSettings::OptimizationResults::mVolumeRemap,
            "Maps old volume constraint index to new volume constraint index")
        .def_rw("skinned_remap", &SoftBodySharedSettings::OptimizationResults::mSkinnedRemap,
            "Maps old skinned constraint index to new skinned constraint index");

    nb::class_<SoftBodySharedSettings::Vertex> vertexCls(softBodySharedSettingsCls, "Vertex",
        "A vertex is a particle, the data in this structure is only used during creation of the soft body and not during simulation");
    vertexCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const Float3 &, const Float3 &, float>(), "position"_a, "velocity"_a, "inv_mass"_a = 1.0f) // TODO: default value
        .def_rw("position", &SoftBodySharedSettings::Vertex::mPosition,
            "Initial position of the vertex")
        .def_rw("velocity", &SoftBodySharedSettings::Vertex::mVelocity,
            "Initial velocity of the vertex")
        .def_rw("inv_mass", &SoftBodySharedSettings::Vertex::mInvMass,
            "Initial inverse of the mass of the vertex");

    nb::class_<SoftBodySharedSettings::Face> faceCls(softBodySharedSettingsCls, "Face",
        "A face defines the surface of the body");
    faceCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, uint32, uint32, uint32>(), "vertex1"_a, "vertex2"_a, "vertex3"_a, "material_index"_a = 0)
        .def("is_degenerate", &SoftBodySharedSettings::Face::IsDegenerate,
            "Check if this is a degenerate face (a face which points to the same vertex twice)")
        // .def_rw("vertex", &SoftBodySharedSettings::Face::mVertex,
            // "Indices of the vertices that form the face")
        .def_rw("material_index", &SoftBodySharedSettings::Face::mMaterialIndex,
            "Index of the material of the face in SoftBodySharedSettings::mMaterials");

    nb::class_<SoftBodySharedSettings::Edge> edgeCls(softBodySharedSettingsCls, "Edge",
        "An edge keeps two vertices at a constant distance using a spring: |x1 - x2| = rest length");
    edgeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, uint32, float>(), "vertex1"_a, "vertex2"_a, "compliance"_a = 0.0f)
        .def("get_min_vertex_index", &SoftBodySharedSettings::Edge::GetMinVertexIndex,
            "Return the lowest vertex index of this constraint")
        // .def_rw("vertex", &SoftBodySharedSettings::Edge::mVertex,
            // "Indices of the vertices that form the edge")
        .def_rw("rest_length", &SoftBodySharedSettings::Edge::mRestLength,
            "Rest length of the spring")
        .def_rw("compliance", &SoftBodySharedSettings::Edge::mCompliance,
            "Inverse of the stiffness of the spring");

    nb::class_<SoftBodySharedSettings::DihedralBend> dihedralBendCls(softBodySharedSettingsCls, "DihedralBend",
        "/**\n"
        "* A dihedral bend constraint keeps the angle between two triangles constant along their shared edge.\n"
        "*\n"
        "*        x2\n"
        "*       /  \\\n"
        "*      / t0 \\\n"
        "*     x0----x1\n"
        "*      \\ t1 /\n"
        "*       \\  /\n"
        "*        x3\n"
        "*\n"
        "* x0..x3 are the vertices, t0 and t1 are the triangles that share the edge x0..x1\n"
        "*\n"
        "* Based on:\n"
        "* - \"Position Based Dynamics\" - Matthias Muller et al.\n"
        "* - \"Strain Based Dynamics\" - Matthias Muller et al.\n"
        "* - \"Simulation of Clothing with Folds and Wrinkles\" - R. Bridson et al.\n"
        "*/");
    dihedralBendCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, uint32, uint32, uint32, float>(), "vertex1"_a, "vertex2"_a, "vertex3"_a, "vertex4"_a, "compliance"_a = 0.0f)
        .def("get_min_vertex_index", &SoftBodySharedSettings::DihedralBend::GetMinVertexIndex,
            "Return the lowest vertex index of this constraint")
        // .def_rw("vertex", &SoftBodySharedSettings::DihedralBend::mVertex,
            // "Indices of the vertices of the 2 triangles that share an edge (the first 2 vertices are the shared edge)")
        .def_rw("compliance", &SoftBodySharedSettings::DihedralBend::mCompliance,
            "Inverse of the stiffness of the constraint")
        .def_rw("initial_angle", &SoftBodySharedSettings::DihedralBend::mInitialAngle,
            "Initial angle between the normals of the triangles (pi - dihedral angle).");

    nb::class_<SoftBodySharedSettings::Volume> volumeCls(softBodySharedSettingsCls, "Volume",
        "Volume constraint, keeps the volume of a tetrahedron constant");
    volumeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, uint32, uint32, uint32, float>(), "vertex1"_a, "vertex2"_a, "vertex3"_a, "vertex4"_a, "compliance"_a = 0.0f)
        .def("get_min_vertex_index", &SoftBodySharedSettings::Volume::GetMinVertexIndex,
            "Return the lowest vertex index of this constraint")
        // .def_rw("vertex", &SoftBodySharedSettings::Volume::mVertex,
            // "Indices of the vertices that form the tetrhedron")
        .def_rw("six_rest_volume", &SoftBodySharedSettings::Volume::mSixRestVolume,
            "6 times the rest volume of the tetrahedron (calculated by CalculateVolumeConstraintVolumes())")
        .def_rw("compliance", &SoftBodySharedSettings::Volume::mCompliance,
            "Inverse of the stiffness of the constraint");

    nb::class_<SoftBodySharedSettings::InvBind> invBindCls(softBodySharedSettingsCls, "InvBind",
        "An inverse bind matrix take a skinned vertex from its bind pose into joint local space");
    invBindCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, Mat44Arg>(), "joint_index"_a, "inv_bind"_a)
        .def_rw("joint_index", &SoftBodySharedSettings::InvBind::mJointIndex,
            "Joint index to which this is attached")
        .def_rw("inv_bind", &SoftBodySharedSettings::InvBind::mInvBind,
            "The inverse bind matrix, this takes a vertex in its bind pose (Vertex::mPosition) to joint local space");

    nb::class_<SoftBodySharedSettings::SkinWeight> skinWeightCls(softBodySharedSettingsCls, "SkinWeight",
        "A joint and its skin weight");
    skinWeightCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, float>(), "inv_bind_index"_a, "weight"_a)
        .def_rw("inv_bind_index", &SoftBodySharedSettings::SkinWeight::mInvBindIndex,
            "Index in mInvBindMatrices")
        .def_rw("weight", &SoftBodySharedSettings::SkinWeight::mWeight,
            "Weight with which it is skinned");

    nb::class_<SoftBodySharedSettings::Skinned> skinnedCls(softBodySharedSettingsCls, "Skinned",
        "A constraint that skins a vertex to joints and limits the distance that the simulated vertex can travel from this vertex");
    skinnedCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, float, float, float>(), "vertex"_a, "max_distance"_a, "back_stop_distance"_a, "back_stop_radius"_a)
        .def("normalize_weights", &SoftBodySharedSettings::Skinned::NormalizeWeights,
            "Normalize the weights so that they add up to 1")
        .def_ro_static("C_MAX_SKIN_WEIGHTS", &SoftBodySharedSettings::Skinned::cMaxSkinWeights,
            "Maximum number of skin weights")
        .def_rw("vertex", &SoftBodySharedSettings::Skinned::mVertex,
            "Index in mVertices which indicates which vertex is being skinned")
        // .def_rw("weights", &SoftBodySharedSettings::Skinned::mWeights,
            // "Skin weights, the bind pose of the vertex is assumed to be stored in Vertex::mPosition. The first weight that is zero indicates the end of the list. Weights should add up to 1.")
        .def_rw("max_distance", &SoftBodySharedSettings::Skinned::mMaxDistance,
            "Maximum distance that this vertex can reach from the skinned vertex, disabled when FLT_MAX. 0 when you want to hard skin the vertex to the skinned vertex.")
        .def_rw("back_stop_distance", &SoftBodySharedSettings::Skinned::mBackStopDistance,
            "Disabled if mBackStopDistance >= mMaxDistance. The faces surrounding mVertex determine an average normal. mBackStopDistance behind the vertex in the opposite direction of this normal, the back stop sphere starts. The simulated vertex will be pushed out of this sphere and it can be used to approximate the volume of the skinned mesh behind the skinned vertex.")
        .def_rw("back_stop_radius", &SoftBodySharedSettings::Skinned::mBackStopRadius,
            "Radius of the backstop sphere. By default this is a fairly large radius so the sphere approximates a plane.")
        .def_rw("normal_info", &SoftBodySharedSettings::Skinned::mNormalInfo,
            "Information needed to calculate the normal of this vertex, lowest 24 bit is start index in mSkinnedConstraintNormals, highest 8 bit is number of faces (generated by CalculateSkinnedConstraintNormals())");

    nb::class_<SoftBodySharedSettings::LRA> lRACls(softBodySharedSettingsCls, "LRA",
        "A long range attachment constraint, this is a constraint that sets a max distance between a kinematic vertex and a dynamic vertex\n"
        "See: \"Long Range Attachments - A Method to Simulate Inextensible Clothing in Computer Games\", Tae-Yong Kim, Nuttapong Chentanez and Matthias Mueller-Fischer");
    lRACls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint32, uint32, float>(), "vertex1"_a, "vertex2"_a, "max_distance"_a)
        .def("get_min_vertex_index", &SoftBodySharedSettings::LRA::GetMinVertexIndex,
            "Return the lowest vertex index of this constraint")
        // .def_rw("vertex", &SoftBodySharedSettings::LRA::mVertex,
            // "The vertices that are connected. The first vertex should be kinematic, the 2nd dynamic.")
        .def_rw("max_distance", &SoftBodySharedSettings::LRA::mMaxDistance,
            "The maximum distance between the vertices");

    nb::enum_<SoftBodySharedSettings::EBendType>(softBodySharedSettingsCls, "EBendType",
        "Which type of bend constraint should be created")
        .value("NONE", SoftBodySharedSettings::EBendType::None,
            "No bend constraints will be created")
        .value("DISTANCE", SoftBodySharedSettings::EBendType::Distance,
            "A simple distance constraint")
        .value("DIHEDRAL", SoftBodySharedSettings::EBendType::Dihedral,
            "A dihedral bend constraint (most expensive, but also supports triangles that are initially not in the same plane)");

    nb::enum_<SoftBodySharedSettings::ELRAType>(softBodySharedSettingsCls, "ELRAType",
        "The type of long range attachment constraint to create")
        .value("NONE", SoftBodySharedSettings::ELRAType::None,
            "Don't create a LRA constraint")
        .value("EUCLIDEAN_DISTANCE", SoftBodySharedSettings::ELRAType::EuclideanDistance,
            "Create a LRA constraint based on Euclidean distance between the closest kinematic vertex and this vertex")
        .value("GEODESIC_DISTANCE", SoftBodySharedSettings::ELRAType::GeodesicDistance,
            "Create a LRA constraint based on the geodesic distance between the closest kinematic vertex and this vertex (follows the edge constraints)");
}