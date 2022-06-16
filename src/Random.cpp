#include "Random.h"

std::mt19937 Random::s_RandomGenerator;
std::uniform_int_distribution<Random::DistType> Random::s_Distribution;