#include "Common.h"
#include <Jolt/Physics/Body/MassProperties.h>
#include <Jolt/Core/StreamIn.h>
#include <Jolt/Core/StreamOut.h>

void BindMassProperties(nb::module_ &m) {
    nb::class_<MassProperties>(m, "MassProperties",
        "Describes the mass and inertia properties of a body. Used during body construction only.")
        .def("decompose_principal_moments_of_inertia", [](const MassProperties &self) {
            Mat44 rotation;
            Vec3 diagonal;
            bool success = self.DecomposePrincipalMomentsOfInertia(rotation, diagonal);
            return nb::make_tuple(success, rotation, diagonal);

        },  "Using eigendecomposition, decompose the inertia tensor into a diagonal matrix D and a right-handed rotation matrix R so that the inertia tensor is \\f$R \\: D \\: R^{-1}\\f$.\n"
            "Args:\n"
            "    rotation (Mat44): The rotation matrix R.\n"
            "    diagonal (Vec3): The diagonal of the diagonal matrix D.\n"
            "Returns:\n"
            "    bool: True if successful, false if failed.\n"
            "References: \n"
            "    https://en.wikipedia.org/wiki/Moment_of_inertia section 'Principal axes'")

        .def("set_mass_and_inertia_of_solid_box", &MassProperties::SetMassAndInertiaOfSolidBox,
            "box_size"_a, "density"_a, "Set the mass and inertia of a box with edge size inBoxSize and density inDensity")

        .def("scale_to_mass", &MassProperties::ScaleToMass, "mass"_a,
            "Set the mass and scale the inertia tensor to match the mass")

        .def_static("get_equivalent_solid_box_size", &MassProperties::sGetEquivalentSolidBoxSize,
            "mass"_a, "inertia_diagonal"_a, "Calculates the size of the solid box that has an inertia tensor diagonal inInertiaDiagonal")

        .def("rotate", &MassProperties::Rotate, "rotation"_a, "Rotate the inertia by 3x3 matrix inRotation")
        .def("translate", &MassProperties::Translate, "translation"_a, "Translate the inertia by a vector inTranslation")
        .def("scale", &MassProperties::Scale, "scale"_a, "Scale the mass and inertia by inScale, note that elements can be < 0 to flip the shape")

        .def("save_binary_state", &MassProperties::SaveBinaryState, "stream"_a, "Saves the state of this object in binary form to inStream.")
        .def("restore_binary_state", &MassProperties::RestoreBinaryState, "stream"_a, "Restore the state of this object from inStream.")

        .def_rw("mass", &MassProperties::mMass, "Mass of the shape (kg)")
        .def_rw("inertia", &MassProperties::mInertia, "Inertia tensor of the shape (kg m^2)");
}