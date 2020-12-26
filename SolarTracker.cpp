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
        return (m_topLeftVal + m_topRightVal) - (m_bottomLeftVal + m_bottomRightVal);
    }

    // calculates a smooth PWM transition
    // modifies axis and pwmTarget in-place
    void Tracker::smoothPwm(AxisData& axis, uint8_t& pwmTarget)
    {
        uint8_t maxChange;
        if (pwmTarget > axis.lastPwmValue)
        {
            maxChange = (axis.lastPwmValue + m_maxPwmStep);
            if (maxChange < pwmTarget)
            {
                pwmTarget = maxChange;
            }
        }
        else
        {
            maxChange = (axis.lastPwmValue - m_maxPwmStep);
            if (maxChange > pwmTarget)
            {
                pwmTarget = maxChange;
            }
        }

        if (pwmTarget < axis.minPwmValue && pwmTarget > 0)
        {
            pwmTarget = axis.minPwmValue;
        }

        axis.lastPwmValue = pwmTarget;
    }

    // smoothes motion in any direction
    void Tracker::execSmooth(AxisData& axis, L298N_Driver::Command targetCommand, uint8_t targetSpeed)
    {
        // slow to zero if direction is different and last command was not Off
        if (targetCommand != axis.lastCommand && axis.lastCommand != L298N_Driver::Command::Off)
        {
            // if min PWM was reached last time, assume it's safe to turn around
            if (axis.lastPwmValue == axis.minPwmValue)
            {
                axis.lastCommand = L298N_Driver::Command::Off;
                axis.lastPwmValue = 0;
                m_driver->exec(axis.channel, axis.lastCommand, axis.lastPwmValue);
            }
            else
            {
                // min PWM not yet reached -> deccelerate
                targetSpeed = 0;
            }
        }

        // smooth & execute
        smoothPwm(axis, targetSpeed);
        m_driver->exec(axis.channel, axis.lastCommand, axis.lastPwmValue);
    }

    // this method modifies error in-place
    void Tracker::executeCorrection(AxisData& axis, int16_t& error)
    {
        L298N_Driver::Command direction;
        uint8_t pwmTarget;
        bool isNegative = (error < 0);

        if (isNegative)
        {
            error *= -1;
            direction = L298N_Driver::Command::Negative;
        }
        else
        {
            direction = L298N_Driver::Command::Positive;
        }

        if (error > m_tolerance)
        {
            pwmTarget = calcPwmValue((uint16_t&)error);
        }
        else
        {
            pwmTarget = 0;
            direction = L298N_Driver::Command::Off;
        }

#ifdef SOLAR_TRACKER_DEBUG
        Serial.print("[SolarTracker] executeCorrection(): axis Ch");
        Serial.print((uint8_t)axis.channel);
        Serial.print(", isNegative ");
        Serial.print(isNegative);
        Serial.print(", pwmTarget ");
        Serial.print(pwmTarget);
        Serial.print(", dir ");
        Serial.print((uint8_t)direction);
        Serial.print(", oldPwmVal ");
        Serial.println(axis.lastPwmValue);
#endif 
        // execute
        execSmooth(axis, direction, pwmTarget);

#ifdef SOLAR_TRACKER_DEBUG
        Serial.print("[SolarTracker] executeCorrection(): newPwmVal ");
        Serial.println(axis.lastPwmValue);
#endif
    }

    void Tracker::autoAdjust()
    {
        // get measurement data
        readLdrs();

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

        // if there is not enough light, stop movement
        if (sum < m_shadowLevel)
        {
            execSmooth(m_aziAxis, L298N_Driver::Command::Off, 0);
            execSmooth(m_eleAxis, L298N_Driver::Command::Off, 0);
            return;
        }

        // calculate errors
        int16_t azimuthError = calcAzimuthError();
        int16_t elevationError = calcElevationError();

        // Azimuth correction
        executeCorrection(m_aziAxis, azimuthError);

        // Elevation correction
        executeCorrection(m_eleAxis, elevationError);
    }

    uint8_t Tracker::calcPwmValue(uint16_t& error)
    {
        // apply P-factor
        uint16_t value = error << m_Kp_shift;

        // upper limit
        if (value > 255)
        {
            value = 255;
        }

        // convert and return
        return (uint8_t)(value & 0x00FF);
    }

    void Tracker::manualAdjust(Axis axis, Direction direction)
    {
        AxisData* axisData;
        switch (axis)
        {
        case Axis::Azimuth:
            axisData = &m_aziAxis;
            break;
        case Axis::Elevation:
            axisData = &m_eleAxis;
            break;
        default:
            return;
        }

        // execute movement
        switch (direction)
        {
        case Direction::Positive:
            execSmooth(*axisData, L298N_Driver::Command::Positive, m_speedManual);
            break;

        case Direction::Negative:
            execSmooth(*axisData, L298N_Driver::Command::Negative, m_speedManual);
            break;

        case Direction::Stop:
        default:
            execSmooth(*axisData, L298N_Driver::Command::Off, 0);
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
        result.limitSwAziPos = m_driver->sense(m_aziAxis.channel, L298N_Driver::Command::Positive);
        result.limitSwAziNeg = m_driver->sense(m_aziAxis.channel, L298N_Driver::Command::Negative);
        result.limitSwElePos = m_driver->sense(m_eleAxis.channel, L298N_Driver::Command::Positive);
        result.limitSwEleNeg = m_driver->sense(m_eleAxis.channel, L298N_Driver::Command::Negative);

        return result;
    }
} // namespace
