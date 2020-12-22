#include "SolarTracker.hpp"
#include "L298N_Driver.hpp"

namespace SolarTracker
{
    void Tracker::readLdrs()
    {
        m_topLeftVal = analogRead(m_config.ldr_topLeftPin);
        m_topRightVal = analogRead(m_config.ldr_topRightPin);
        m_bottomLeftVal = analogRead(m_config.ldr_bottomLeftPin);
        m_bottomRightVal = analogRead(m_config.ldr_bottomRightPin);
    }

    int16_t Tracker::calcAzimuthError() const
    {
        return (m_topLeftVal + m_bottomLeftVal) - (m_topRightVal + m_bottomRightVal);
    }

    int16_t Tracker::calcElevationError() const
    {
        return (m_topLeftVal + m_topRightVal) - (m_topRightVal + m_bottomLeftVal);
    }

    void Tracker::smoothPwm(uint8_t* lastPwmValue, uint8_t& pwmTarget)
    {
        uint8_t maxChange;
        if (pwmTarget > *lastPwmValue)
        {
            maxChange = (*lastPwmValue + m_maxPwmStep);
            if (maxChange > pwmTarget)
            {
                pwmTarget = maxChange;
            }
        }
        else
        {
            maxChange = (*lastPwmValue - m_maxPwmStep);
            if (maxChange < pwmTarget)
            {
                pwmTarget = maxChange;
            }
        }

        *lastPwmValue = pwmTarget;
    }

    void Tracker::executeCorrection(int16_t* error, uint8_t* lastPwmValue, L298N_Driver::Channel channel)
    {
        L298N_Driver::Command direction;
        uint8_t pwmTarget;
        bool isNegative = (*error < 0);

        if (isNegative)
        {
            *error *= -1;
            direction = L298N_Driver::Command::Negative;
        }
        else
        {
            direction = L298N_Driver::Command::Positive;
        }

        if (*error > m_tolerance)
        {
            pwmTarget = calcPwmValue((uint16_t&)*error);
        }
        else
        {
            pwmTarget = 0;
            direction = L298N_Driver::Command::Off;
        }
#ifdef SOLAR_TRACKER_DEBUG
        Serial.print("[SolarTracker] executeCorrection(): isNegative ");
        Serial.print(isNegative);
        Serial.print(", pwmTarget ");
        Serial.print(pwmTarget);
        Serial.print(", dir ");
        Serial.print((uint8_t)direction);
        Serial.print(", oldPwmVal ");
        Serial.print(*lastPwmValue);
#endif 

        // modify last value in-place
        smoothPwm(lastPwmValue, pwmTarget);

#ifdef SOLAR_TRACKER_DEBUG
        Serial.print(", newPwmVal ");
        Serial.println(*lastPwmValue);
#endif

        // execute
        m_driver->exec(channel, direction, *lastPwmValue);
    }

    void Tracker::autoAdjust()
    {
        // get measurement data
        readLdrs();

        // check plausibility
        // TODO
        // calculate sum for shadow check
        uint16_t sum = m_topLeftVal + m_topRightVal + m_bottomLeftVal + m_bottomRightVal;

#ifdef SOLAR_TRACKER_DEBUG
        Serial.print("[SolarTracker] autoAdjust(): TL ");
        Serial.print(m_topLeftVal);
        Serial.print(", TR ");
        Serial.print(m_topRightVal);
        Serial.print(",: BL ");
        Serial.print(m_bottomLeftVal);
        Serial.print(", BR ");
        Serial.print(m_bottomRightVal);
        Serial.print(", sum ");
        Serial.println(sum);
#endif 

        // if there is not enough light, skip movement
        if (sum < m_shadowLevel)
        {
            return;
        }

        // calculate errors
        int16_t azimuthError = calcAzimuthError();
        int16_t elevationError = calcElevationError();

        // Azimuth correction
        executeCorrection(&azimuthError, &m_lastPwmValue_azi, m_azimuthChannel);

        // Elevation correction
        executeCorrection(&elevationError, &m_lastPwmValue_ele, m_elevationChannel);
    }

    uint8_t Tracker::calcPwmValue(uint16_t& error) const
    {
        // apply P-factor
        uint16_t value = error >> m_Kp_shift;

        // saturate
        if (value > 255)
        {
            value = 255;
        }

        // convert and return
        return (uint8_t)(value & 0x00FF);
    }

    void Tracker::manualAdjust(Axis axis, Direction direction)
    {
        L298N_Driver::Channel channel;
        uint8_t* lastPwmValue;
        switch (axis)
        {
        case Axis::Azimuth:
            channel = m_azimuthChannel;
            lastPwmValue = &m_lastPwmValue_azi;
            break;
        case Axis::Elevation:
            channel = m_elevationChannel;
            lastPwmValue = &m_lastPwmValue_ele;
            break;
        default:
            return;
        }

        // do not allow manual movement before stopping automatic movement
        /*
        if (*lastPwmValue > 0)
        {
            executeCorrection(0, lastPwmValue, channel);
            return;
        }
        */

        // execute movement
        switch (direction)
        {
        case Direction::Positive:
            m_driver->exec(channel, L298N_Driver::Command::Positive, m_speedManual);
            break;

        case Direction::Negative:
            m_driver->exec(channel, L298N_Driver::Command::Negative, m_speedManual);
            break;

        case Direction::Stop:
        default:
            m_driver->exec(channel, L298N_Driver::Command::Off);
            break;
        }
    }

    InputInfo Tracker::getInputInfo()
    {
        InputInfo result = InputInfo();

        // get LDR info
        readLdrs();
        result.ldrValTopLeft = m_topLeftVal;
        result.ldrValTopRight = m_topRightVal;
        result.ldrValBotLeft = m_bottomLeftVal;
        result.ldrValBotRight = m_bottomRightVal;

        // get limit switch data
        result.limitSwAziPos = m_driver->sense(m_azimuthChannel, L298N_Driver::Command::Positive);
        result.limitSwAziNeg = m_driver->sense(m_azimuthChannel, L298N_Driver::Command::Negative);
        result.limitSwElePos = m_driver->sense(m_elevationChannel, L298N_Driver::Command::Positive);
        result.limitSwEleNeg = m_driver->sense(m_elevationChannel, L298N_Driver::Command::Negative);

        return result;
    }
} // namespace
