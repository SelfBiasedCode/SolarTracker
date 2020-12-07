#ifndef SOLAR_TRACKER_CONFIG_H
#define SOLAR_TRACKER_CONFIG_H

#include "stdint.h"
struct SolarTrackerConfig
{
    uint8_t ldr_topLeftPin;
    uint8_t ldr_topRightPin;
    uint8_t ldr_bottomLeftPin;
    uint8_t ldr_bottomRightPin;

    uint8_t motor_azimuth_signalPin;
    uint8_t motor_azimuth_positivePin;
    uint8_t motor_azimuth_negativePin;

    uint8_t motor_elevation_signalPin;
    uint8_t motor_elevation_positivePin;
    uint8_t motor_elevation_negativePin;

    uint8_t led_status_pin;
};

#endif
