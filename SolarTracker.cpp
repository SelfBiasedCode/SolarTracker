#include "SolarTracker.h"

void SolarTracker::readLdrs()
{
	m_topLeftVal = analogRead(m_topLeftPin);
	m_topRightVal = analogRead(m_topRightPin);
	m_bottomLeftVal = analogRead(m_bottomLeftPin);
	m_bottomRightVal = analogRead(m_bottomRightPin);
}

int16_t SolarTracker::calcAzimuthError()
{
	return (m_topLeftVal + m_bottomLeftVal) - (m_topRightVal + m_bottomRightVal);
}

int16_t SolarTracker::calcElevationError()
{
	return (m_topLeftVal + m_topRightVal) - (m_topRightVal + m_bottomLeftVal);
}

void SolarTracker::process()
{
	// get measurement data
	readLdrs();

	// calculate errors
	int16_t azimuthError = calcAzimuthError();
	int16_t elevationError = calcElevationError();

	// apply correction
	if (azimuthError > m_tolerance)
	{
		uint8_t pwmValue = calcPwmValue(azimuthError);
		analogWrite(m_azimuthPin, pwmValue);
	}
	else
	{
		// disable motor
		analogWrite(m_azimuthPin, 0);
	}

	if (elevationError > m_tolerance)
	{
		uint8_t pwmValue = calcPwmValue(elevationError);
		analogWrite(m_elevationPin, pwmValue);
	}
	else
	{
		// disable motor
		analogWrite(m_elevationPin, 0);
	}
}

uint8_t SolarTracker::calcPwmValue(int16_t & error) const
{
	// use slow speed by default
	uint8_t pwmValue = m_speedSlow;

	// switch to high speed if above switchover point
	if (error > m_speedSwitchover)
	{
		pwmValue = m_speedFast;
	}

	return pwmValue;
}
