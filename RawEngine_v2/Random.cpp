//
// Created by Lars on 8-5-2026.
//

#include "Random.h"
#pragma once
#include <cstdlib>


float Random::RandomFloat(float min, float max)
{
    return min + static_cast<float>(rand()) /
    (static_cast<float>(RAND_MAX / (max - min)));
}

