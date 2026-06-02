#include "FloodSim.hpp"
#include <iostream>

FloodSim::FloodSim(const int a) : a(a) {}

void FloodSim::show() const { std::cout << "value: " << a << std::endl; }
