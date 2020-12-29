#ifndef SOLAR_TRACKER_CONFIG_H
#define SOLAR_TRACKER_CONFIG_H

#include "stdint.h"

namespace SolarTracker
{
    // struct to allow for const initialization
    struct Config
    {
        const uint8_t ldr_topLeftPin;
        const uint8_t ldr_topRightPin;
        const uint8_t ldr_bottomLeftPin;
        const uint8_t ldr_bottomRightPin;

        const uint8_t motor_azimuth_signalPin;
        const uint8_t motor_azimuth_positivePin;
        const uint8_t motor_azimuth_negativePin;
        const uint8_t motor_azimuth_minPwm;

        const uint8_t motor_elevation_signalPin;
        const uint8_t motor_elevation_positivePin;
        const uint8_t motor_elevation_negativePin;
        const uint8_t motor_elevation_minPwm;
    };
}

#endif
