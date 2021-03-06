#ifndef OUTPUT_INFO_H
#define OUTPUT_INFO_H

#include <stdint.h>
#include "L298N_Driver.hpp"

namespace SolarTracker
{
    class OutputInfo
    {
    public:
        OutputInfo(const L298N_Driver::Command aziCmd, const uint8_t aziSpeed, const L298N_Driver::Command eleCmd, const uint8_t eleSpeed) :
            azimuthCommand(aziCmd), azimuthSpeed(aziSpeed), elevationCommand(eleCmd), elevationSpeed(eleSpeed)
        {
        }

        const L298N_Driver::Command azimuthCommand;
        const uint8_t azimuthSpeed;
        const L298N_Driver::Command elevationCommand;
        const uint8_t elevationSpeed;
    };
}

#endif
