#ifndef INPUT_INFO_H
#define INPUT_INFO_H

#include <stdint.h>

namespace SolarTracker
{
    class InputInfo
    {
    public:
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
