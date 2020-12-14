#include "SolarTracker.hpp"
#include "L298N_Driver.hpp"

void SolarTracker::readLdrs()
{
    m_topLeftVal = analogRead(m_config.ldr_topLeftPin);
    m_topRightVal = analogRead(m_config.ldr_topRightPin);
    m_bottomLeftVal = analogRead(m_config.ldr_bottomLeftPin);
    m_bottomRightVal = analogRead(m_config.ldr_bottomRightPin);
}

int16_t SolarTracker::calcAzimuthError() const
{
    return (m_topLeftVal + m_bottomLeftVal) - (m_topRightVal + m_bottomRightVal);
}

int16_t SolarTracker::calcElevationError() const
{
    return (m_topLeftVal + m_topRightVal) - (m_topRightVal + m_bottomLeftVal);
}

void SolarTracker::smoothPwm(uint8_t* lastPwmValue, uint8_t& pwmTarget)
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

void SolarTracker::executeCorrection(int16_t* error, uint8_t* lastPwmValue, L298N_Driver::Channel channel)
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

    // modify last value in-place
    smoothPwm(lastPwmValue, pwmTarget);

    // execute
    m_driver->exec(channel, direction, *lastPwmValue);
}

void SolarTracker::autoAdjust()
{
    // get measurement data
    readLdrs();

    // check plausibility
    // TODO
    // calculate sum for shadow check
    uint16_t sum = m_topLeftVal + m_topRightVal + m_bottomLeftVal + m_bottomRightVal;

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

uint8_t SolarTracker::calcPwmValue(uint16_t& error) const
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

void SolarTracker::manualAdjust(Axis axis, Direction direction)
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
