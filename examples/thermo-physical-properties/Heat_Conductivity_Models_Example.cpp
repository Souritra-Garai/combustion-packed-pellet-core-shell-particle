#include <fstream>
#include <iostream>

#include "utilities/File_Generator.hpp"

#include "substances/Aluminium.hpp"
#include "substances/Nickel.hpp"
#include "substances/NickelAluminide.hpp"
#include "substances/Argon.hpp"

#include "thermo-physical-properties/Core-Shell-Combustion-Particle.hpp"
#include "thermo-physical-properties/Heat_Conductivity_Models.hpp"

double core_radius = 32.5E-6;
double overall_radius = 39.5E-6;

int main(int argc, char const *argv[])
{
    CoreShellCombustionParticle<double>::setUpCoreShellCombustionParticle(
        Aluminium, Nickel, NickelAluminide,
        overall_radius, core_radius
    );

    CoreShellCombustionParticle<double> Ni_clad_Al_particle;

    FileGenerator file_generator;

    std::ofstream file = file_generator.getCSVFile("thermal_conductivity_pellet");

    file << "Particle Volume Fraction,ME1,CC,EMT,ME2" << std::endl;

    double particle_thermal_conductivity = Ni_clad_Al_particle.getThermalConductivity(298);
    double fluid_thermal_conductivity = Argon.getThermalConductivity(298);

    std::cout << "Particle Thermal Conductivity : " << particle_thermal_conductivity << std::endl;
    std::cout << "Fluid Thermal Conductivity : " << fluid_thermal_conductivity << std::endl;

    for (double particle_volume_fraction = 0.001; particle_volume_fraction <= 0.999; particle_volume_fraction += 0.001)
    {
        file << particle_volume_fraction << ',';
        file << getMaxwellEucken1HeatConudctivity(particle_volume_fraction, particle_thermal_conductivity, fluid_thermal_conductivity) << ',';
        file << getCoContinuousHeatConductivity(particle_volume_fraction, particle_thermal_conductivity, fluid_thermal_conductivity) << ',';
        file << getBruggemanHeatConductivity(particle_volume_fraction, particle_thermal_conductivity, fluid_thermal_conductivity) << ',';
        file << getMaxwellEucken2HeatConudctivity(particle_volume_fraction, particle_thermal_conductivity, fluid_thermal_conductivity) << std::endl;
    }

    file.close();
    return 0;
}