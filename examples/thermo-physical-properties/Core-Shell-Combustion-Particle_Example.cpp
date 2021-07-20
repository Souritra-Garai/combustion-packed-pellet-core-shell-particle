/**
 * @file Core-Shell-Combustion-Particle_Example.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include <iostream>

#include "thermo-physical-properties/Core-Shell-Combustion-Particle.hpp"

Substance<float> Al(2700, 897, 26.98, 239);
Substance<float> Ni(8902, 440, 58.69, 90.7);
Substance<float> NiAl(5900, 717, 85.675, 115, -118.4);

float core_radius = 32.5E-6;
float overall_radius = 39.5E-6;

CoreShellCombustionParticle<float> AB_particle;

int main(int argc, char const *argv[])
{
    AB_particle.setUpCoreShellParticle(
        Al, Ni, overall_radius, core_radius
    );
    std::cout << "Enthalpy\t:\t" << AB_particle.getEnthalpy(350) << "\tJ / kg" << std::endl;
    AB_particle.printProperties(std::cout);
    return 0;
}
