/**
 * @file Packed-Pellet.cpp
 * @author Souritra Garai (souritra.garai@iitgn.ac.in)
 * @brief Implementation details for the class PackedPellet
 * @version 0.1
 * @date 2021-07-31
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// For definition of PackedPellet class
#include "thermo-physical-properties/Packed-Pellet.hpp"

// Required for pow function
#include <math.h>

// Required for functions to calculate heat conductivity
#include "thermo-physical-properties/Heat_Conductivity_Models.hpp"

/*****************************************************************************************************/
// Instantiation of static members
// Only one copy of these variables are shared across all classes

// Length of pellet in \f$ m \f$
template<typename real_t> real_t PackedPellet<real_t>::_length = 1.0;
// Diameter of pellet in \f$ m \f$
template<typename real_t> real_t PackedPellet<real_t>::_diameter = 1.0;

// Ambient heat loss parameters
// Convective heat transfer coefficient
template<typename real_t> real_t PackedPellet<real_t>::_convective_heat_transfer_coefficient = 0;
// Emissivity
template<typename real_t> real_t PackedPellet<real_t>::_radiative_emissivity = 0;

// Ambient temperature in K
template<typename real_t> real_t PackedPellet<real_t>::_ambient_temperature = 298.0;

// Ignition temperature of the energetic particles in K
template<typename real_t> real_t PackedPellet<real_t>::_ignition_temperature = 298.0;

// Substance filling the voids in the packed pellet
template<typename real_t> Substance<real_t> * PackedPellet<real_t>::_degassing_fluid;

/*****************************************************************************************************/
// Definitions of Static Member Functions

template<typename real_t>
void PackedPellet<real_t>::setPelletDimensions(
    real_t length,
    real_t diameter
) {
    // Set static member variable - length of the pellets
    _length = length;
    // Set static member variable - diameter of the pellets
    _diameter = diameter;
}

template<typename real_t>
void PackedPellet<real_t>::setAmbientHeatLossParameters(
    real_t convective_heat_transfer_coefficient,
    real_t radiative_emissivity
) {
    // Set static member variable - convective heat transfer coefficient
    _convective_heat_transfer_coefficient = convective_heat_transfer_coefficient;
    // Set static member variable - emissivity for radiative heat loss
    _radiative_emissivity = radiative_emissivity;
}

template<typename real_t>
void PackedPellet<real_t>::setTemperatureParameters(
    real_t ignition_temperature,
    real_t ambient_temperature
) {
    // Set static member variable - ignition temperature
    _ignition_temperature = ignition_temperature;
    // Set static member variable - ambient temperature
    _ambient_temperature = ambient_temperature;
}

template<typename real_t>
void PackedPellet<real_t>::setDegassingFluid(Substance<real_t> &degassing_fluid)
{
    // Set static member variable - Substance filling the voids of the pellet
    _degassing_fluid = &degassing_fluid;
}

template<typename real_t>
real_t PackedPellet<real_t>::calcParticleMassFractions(
    real_t particle_volume_fractions
) {
    // Get the density of a defualt Core-Shell Combustion Particle instance
    // This assumes the static members of the CoreShellCombustionParticle class
    // have been initialized
    real_t initial_particle_density = CoreShellCombustionParticle<real_t>().getDensity(298.15);
    
    // Mass of particles present in unit volume of pellet = \f$ \phi_P \rho_p \f$
    // Mass of degassing fluid in unit volume of pellet = \f$ (1 - \phi_P) \rho_P \f$
    // Mass fractions of particles = Mass of particles / Mass of unit volume of pellet
    return particle_volume_fractions * initial_particle_density / (
        particle_volume_fractions           * initial_particle_density + 
        (1.0 - particle_volume_fractions)   * _degassing_fluid->getDensity(298.15)
    );
}

template<typename real_t>
real_t PackedPellet<real_t>::calcDensity(real_t particle_volume_fractions)
{
    // Get the density of a defualt Core-Shell Combustion Particle instance
    // This assumes the static members of the CoreShellCombustionParticle class
    // have been initialized
    real_t initial_particle_density = CoreShellCombustionParticle<real_t>().getDensity(298.15);

    // For mean pellet density, take the volume fractions weighted average of densities
    // of the particle and the degassing fluid
    return particle_volume_fractions  * initial_particle_density +
    (1.0 - particle_volume_fractions) * _degassing_fluid->getDensity(298.15);
}

/*****************************************************************************************************/
// Constructors and destructors

template<typename real_t>
PackedPellet<real_t>::PackedPellet(
    real_t particle_volume_fractions
) : // Mem Initialization list for initialising the constant members
    _density(calcDensity(particle_volume_fractions)),
    _particle_volume_fractions(particle_volume_fractions),
    _particle_mass_fractions(calcParticleMassFractions(particle_volume_fractions))
{ 
    // Nothing to do
    ; 
}

/*****************************************************************************************************/
// Defining member functions

template<typename real_t>
real_t PackedPellet<real_t>::getDensity() { return _density; }

template<typename real_t>
real_t PackedPellet<real_t>::getHeatCapacity(CoreShellCombustionParticle<real_t> *ptr_2_particle, real_t T)
{
    // Take mass fractions weighted average of heat capacities of the 
    // particle and th degassing fluid
    return _particle_mass_fractions  * ptr_2_particle->getHeatCapacity(T) +
    (1.0 - _particle_mass_fractions) * _degassing_fluid->getHeatCapacity(T);
}

template<typename real_t>
real_t PackedPellet<real_t>::getThermalConductivity(CoreShellCombustionParticle<real_t> *ptr_2_particle, real_t T)
{    
    // Return the heat conductivity determined using Bruggeman model
    return getBruggemanHeatConductivity(
        _particle_volume_fractions,
        ptr_2_particle->getThermalConductivity(T),
        _degassing_fluid->getThermalConductivity(T)
    );
}

template<typename real_t>
real_t PackedPellet<real_t>::getInternalEnergy(CoreShellCombustionParticle<real_t> *ptr_2_particle, real_t T)
{
    // Take mass fractions weighted average of enthalpies of the 
    // particle and th degassing fluid
    return _particle_mass_fractions  * ptr_2_particle->getInternalEnergy(T) +
    (1.0 - _particle_mass_fractions) * _degassing_fluid->getInternalEnergy(T);
}

template<typename real_t>
void PackedPellet<real_t>::printProperties(std::ostream &output_stream)
{
    output_stream << "Length\t\t:\t" << _length << "\tm" << std::endl;
    output_stream << "Diameter\t:\t" << _diameter << "\tm" << std::endl;

    output_stream << "\nAmbient Heat Loss Parameters" << std::endl;
    output_stream << "Convective Heat Transfer Coefficient\t:\t" << _convective_heat_transfer_coefficient << "\tW/m2-K" << std::endl;
    output_stream << "Radiative emissivity\t\t\t:\t" << _radiative_emissivity << std::endl;
    output_stream << "Ambient Temperature\t\t\t:\t" << _ambient_temperature << "\tK" << std::endl;

    CoreShellCombustionParticle<real_t> particle;

    output_stream << "\nDensity\t\t\t\t:\t" << getDensity() << "\tkg/m3" << std::endl;
    output_stream << "Heat Capacity\t\t\t:\t" << getHeatCapacity(&particle, 298.15) << "\tJ/kg-K" << std::endl;
    output_stream << "Heat Conductivity\t\t:\t" << getThermalConductivity(&particle, 298.15) << "\tW/m-K" << std::endl;

    output_stream << "\nParticle Properties" << std::endl;
    output_stream << "Particle Volume Fraction\t:\t" << _particle_volume_fractions << std::endl;
    output_stream << "Particle Mass Fraction\t:\t" << _particle_mass_fractions << std::endl;
    particle.printProperties(output_stream);

    output_stream << "\nDegassing Fluid" << std::endl;
    output_stream << "Degassing Fluid Volume Fraction\t:\t" << 1.0 - _particle_volume_fractions << std::endl;
    output_stream << "Degassing Fluid Mass Fraction\t:\t" << 1.0 - _particle_mass_fractions << std::endl;
}

/*****************************************************************************************************/
template class PackedPellet<float>;
template class PackedPellet<double>;
template class PackedPellet<long double>;