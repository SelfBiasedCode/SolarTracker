#ifndef AXIS_DATA_HPP
#define AXIS_DATA_HPP

#include <stdint.h>
#include "L298N_Driver.hpp"

namespace SolarTracker
{
    class AxisData
    {
    public:
        AxisData(const uint8_t minPwmVal, const L298N_Driver::Channel channel, L298N_Driver::Command lastCmd = L298N_Driver::Command::Off) :
            lastPwmValue(0), lastCommand(lastCmd), minPwmValue(minPwmVal), channel(channel)
        {
        }
        uint8_t lastPwmValue;
        L298N_Driver::Command lastCommand;
        const uint8_t minPwmValue;
        const L298N_Driver::Channel channel;
    };
} // namespace

#endif