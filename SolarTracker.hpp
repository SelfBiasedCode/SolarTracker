#ifndef SOLAR_TRACKER_H
#define SOLAR_TRACKER_H

#include "SolarTrackerConfig.hpp"
#include "L298N_Driver.hpp"

class SolarTracker
{
public:
    SolarTracker(SolarTrackerConfig config) :m_config(config)
    {
        m_driver = new L298N_Driver(m_config.motor_azimuth_signalPin, m_config.motor_azimuth_positivePin, m_config.motor_azimuth_negativePin, m_config.motor_elevation_signalPin, m_config.motor_elevation_positivePin, m_config.motor_elevation_negativePin);
        m_lastPwmValue_azi = 0;
        m_lastPwmValue_ele = 0;
    }

    enum class Direction : uint8_t { Positive, Negative, Stop };

    void process();
    void manualAzimuth(Direction direction) const;
    void manualElevation(Direction direction) const;

private:
    // methods
    void readLdrs();
    int16_t calcAzimuthError() const;
    int16_t calcElevationError() const;
    uint8_t calcPwmValue(uint16_t& error) const;

    // most recent data
    uint8_t m_topLeftVal;
    uint8_t m_topRightVal;
    uint8_t m_bottomLeftVal;
    uint8_t m_bottomRightVal;
    uint8_t m_lastPwmValue_azi;
    uint8_t m_lastPwmValue_ele;

    // config
    SolarTrackerConfig m_config;

    // driver
    L298N_Driver* m_driver;

    // constants
    static const uint8_t m_tolerance = 10;
    static const uint16_t m_shadowLevel = 100;
    static const uint8_t m_speedManual = 100;
    static const uint8_t m_Kp_shift = 1;
    static const uint8_t m_maxPwmStep = 25;

    static const L298N_Driver::Channel m_azimuthChannel = L298N_Driver::Channel::Ch1;
    static const L298N_Driver::Channel m_elevationChannel = L298N_Driver::Channel::Ch2;
};

#endif
