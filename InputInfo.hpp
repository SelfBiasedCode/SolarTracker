#ifndef INPUT_INFO_HPP
#define INPUT_INFO_HPP

#include <stdint.h>

namespace SolarTracker
{
    struct InputInfo
    {
        uint16_t ldrValTopLeft;
        uint16_t ldrValTopRight;
        uint16_t ldrValBotLeft;
        uint16_t ldrValBotRight;

        bool limitSwAziPos;
        bool limitSwAziNeg;
        bool limitSwElePos;
        bool limitSwEleNeg;
    };
}

#endif
