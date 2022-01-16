#pragma once

#include "Assert.h"

#include <random>

class Random
{
public:
    static void Init()
    {
        s_RandomGenerator.seed(std::random_device()());
    }

    static float Float()
    {
        return ((float)s_Distribution(s_RandomGenerator) / (float)std::numeric_limits<std::mt19937::result_type>::max());
    }

    static bool Bool()
    {
        return Float() < 0.5;
    }

    static unsigned int UInt()
    {
        return (unsigned int)(s_Distribution(s_RandomGenerator));
    }

    static unsigned int UInt(unsigned int min, unsigned int max)
    {
        BL_ASSERT(min < max, "The minimum (", min, ") should not be smaller than the maximum (", max, ").");

        return min + (((unsigned int)s_Distribution(s_RandomGenerator)) % (max - min));
    }

    static int Int()
    {
        return (int)(s_Distribution(s_RandomGenerator));
    }

    static int Int(int min, int max)
    {
        BL_ASSERT(min < max, "The minimum (", min, ") should not be smaller than the maximum (", max, ").");

        return min + (((int)s_Distribution(s_RandomGenerator)) % (max - min));
    }

private:
    static std::mt19937 s_RandomGenerator;
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};