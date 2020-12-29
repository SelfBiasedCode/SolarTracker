#ifndef OUTPUT_INFO_H
#define OUTPUT_INFO_H

#include <stdint.h>
#include "Direction.hpp"

namespace SolarTracker
{
    class OutputInfo
    {
    public:
        OutputInfo(Direction aziDir, uint8_t aziSpeed, Direction eleDir, uint8_t eleSpeed) :
            azimuthDirection(aziDir), azimuthSpeed(aziSpeed), elevationDirection(eleDir), elevationSpeed(eleSpeed) 
        {
        }

        const Direction azimuthDirection;
        const uint8_t azimuthSpeed;
        const Direction elevationDirection;
        const uint8_t elevationSpeed;
    };
}

#endif
