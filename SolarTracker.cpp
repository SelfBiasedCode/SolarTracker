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

void SolarTracker::process()
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

    // apply correction
    if (azimuthError > m_tolerance)
    {
        bool isNegative = (azimuthError < 0);
        L298N_Driver::Command direction;
        if (isNegative)
        {
            azimuthError *= -1;
            direction = L298N_Driver::Command::Negative;
        }
        else
        {
            direction = L298N_Driver::Command::Positive;
        }

        uint8_t pwmValue = calcPwmValue((uint16_t&)azimuthError);

        // this calculation may overflow without negative effects
        uint8_t maxIncrease = (m_lastPwmValue_azi + m_maxPwmStep);
        if (maxIncrease > pwmValue)
        {
            pwmValue = maxIncrease;
        }

        m_lastPwmValue_azi = pwmValue;
        m_driver->exec(m_azimuthChannel, direction, pwmValue);
    }
    else
    {
        // disable motor
        m_driver->exec(m_azimuthChannel, L298N_Driver::Command::Off);
    }

    if (elevationError > m_tolerance)
    {
        bool isNegative = (elevationError < 0);
        L298N_Driver::Command direction;
        if (isNegative)
        {
            elevationError *= -1;
            direction = L298N_Driver::Command::Negative;
        }
        else
        {
            direction = L298N_Driver::Command::Positive;
        }

        uint8_t pwmValue = calcPwmValue((uint16_t&)elevationError);

        // this calculation may overflow without negative effects
        uint8_t maxIncrease = (m_lastPwmValue_ele + m_maxPwmStep);
        if (maxIncrease > pwmValue)
        {
            pwmValue = maxIncrease;
        }
        m_lastPwmValue_ele = pwmValue;
        m_driver->exec(m_elevationChannel, direction, pwmValue);
    }
    else
    {
        // disable motor
        m_driver->exec(m_elevationChannel, L298N_Driver::Command::Off);
    }
}

uint8_t SolarTracker::calcPwmValue(uint16_t& error) const
{
    uint16_t value = error >> m_Kp_shift;
    return (uint8_t)(value & 0x00FF);
}

void SolarTracker::manualAzimuth(Direction direction) const
{
    switch (direction)
    {
    case Direction::Positive:
        m_driver->exec(m_azimuthChannel, L298N_Driver::Command::Positive, m_speedManual);
        break;

    case Direction::Negative:
        m_driver->exec(m_azimuthChannel, L298N_Driver::Command::Negative, m_speedManual);
        break;

    case Direction::Stop:
    default:
        m_driver->exec(m_azimuthChannel, L298N_Driver::Command::Off);
        break;
    }
}

void SolarTracker::manualElevation(Direction direction) const
{
    switch (direction)
    {
    case Direction::Positive:
        m_driver->exec(m_elevationChannel, L298N_Driver::Command::Positive, m_speedManual);
        break;

    case Direction::Negative:
        m_driver->exec(m_elevationChannel, L298N_Driver::Command::Negative, m_speedManual);
        break;

    case Direction::Stop:
    default:
        m_driver->exec(m_elevationChannel, L298N_Driver::Command::Off);
        break;
    }
}
