#ifndef KNOWN_STATES_H
#define KNOWN_STATES_H

#include <stdint.h>

namespace SolarTracker::States
{
    enum class State : uint8_t
    {
        Init = 0,
        Transition,
        Manual,
        Auto,
        Test
    };
}
#endif

