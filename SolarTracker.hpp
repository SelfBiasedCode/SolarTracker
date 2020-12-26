#ifndef SOLAR_TRACKER_H
#define SOLAR_TRACKER_H

#include "SolarConfig.hpp"
#include "InputInfo.hpp"
#include "L298N_Driver.hpp"
#include "AxisData.hpp"

// #define SOLAR_TRACKER_DEBUG

namespace SolarTracker
{
    class Tracker
    {
    public:
        Tracker(SolarConfig config) :m_config(config), m_aziAxis(config.motor_azimuth_minPwm, L298N_Driver::Channel::Ch1), m_eleAxis(config.motor_elevation_minPwm, L298N_Driver::Channel::Ch2)
        {
            // instantiate driver
            m_driver = new L298N_Driver(m_config.motor_azimuth_signalPin, m_config.motor_azimuth_positivePin, m_config.motor_azimuth_negativePin, m_config.motor_elevation_signalPin, m_config.motor_elevation_positivePin, m_config.motor_elevation_negativePin);
        }

        void init()
        {
#ifdef SOLAR_TRACKER_DEBUG
            Serial.println("[SolarTracker] init()");
#endif 

            m_driver->init();
        }

        enum class Direction : uint8_t { Positive, Negative, Stop };
        enum class Axis : uint8_t { Azimuth, Elevation };

        void autoAdjust();
        void manualAdjust(Axis axis, Direction direction);
        InputInfo getInputInfo();

    private:
        // methods
        void readLdrs();
        int16_t calcAzimuthError() const;
        int16_t calcElevationError() const;
        uint8_t calcPwmValue(uint16_t& error);
        void smoothPwm(AxisData& axis, uint8_t& pwmTarget);
        void execSmooth(AxisData& axis, L298N_Driver::Command direction, uint8_t speed);
        void executeCorrection(AxisData& axis, int16_t& error);
        
        // most recent data
        uint16_t m_topLeftVal;
        uint16_t m_topRightVal;
        uint16_t m_bottomLeftVal;
        uint16_t m_bottomRightVal;

        // config
        SolarConfig m_config;
        AxisData m_aziAxis;
        AxisData m_eleAxis;

        // driver
        L298N_Driver* m_driver;

        // controller constants
        static const uint8_t m_tolerance = 10;
        static const uint16_t m_shadowLevel = 100;
        static const uint8_t m_Kp_shift = 0;

        // PWM constants
        static const uint8_t m_speedManual = 100;
        static const uint8_t m_maxPwmStep = 25;
    };
}
#endif
