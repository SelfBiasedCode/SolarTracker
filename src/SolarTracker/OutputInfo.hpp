#ifndef OUTPUT_INFO_HPP
#define OUTPUT_INFO_HPP

#include <stdint.h>
#include "Direction.hpp"
#include "Tracker.hpp"

namespace SolarTracker
{
    class OutputInfo
    {
    public:
        OutputInfo(Direction aziDir, Direction eleDir) :
            azimuthDirection(aziDir), elevationDirection(eleDir)
        {
        }

        const Direction azimuthDirection;
        const Direction elevationDirection;
    };
}

#endif
