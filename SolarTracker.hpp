#ifndef SOLAR_TRACKER_H
#define SOLAR_TRACKER_H

class SolarTracker
{
public:
	SolarTracker(uint8_t azimuthMotorPin, uint8_t elevationMotorPin, uint8_t topLeftLdrPin, uint8_t topRightLdrPin, uint8_t bottomLeftLdrPin, uint8_t bottomRightLdrPin, uint8_t statusLedPin) :
		m_azimuthPin(azimuthMotorPin), m_elevationPin(elevationMotorPin), m_topLeftPin(topLeftLdrPin), topRightLdrPin(topRightLdrPin), m_bottomLeftPin(bottomLeftLdrPin), m_bottomRightPin(bottomRightLdrPin), m_statusLedPin(statusLedPin)
	{}

	void process();
	void moveWest();
	void moveEast();
	void moveUp();
	void moveDown();

private:
	void readLdrs();

	int16_t calcAzimuthError();
	int16_t calcElevationError();
	uint8_t calcPwmValue(int16_t& error)

	// most recent data
	uint8_t m_topLeftVal;
	uint8_t m_topRightVal;
	uint8_t m_bottomLeftVal;
	uint8_t m_bottomRightVal;

	/* pins */
	// LDRs
	uint8_t m_topLeftPin;
	uint8_t m_topRightPin;
	uint8_t m_bottomLeftPin;
	uint8_t m_bottomRightPin;

	// motors
	uint8_t m_azimuthPin;
	uint8_t m_elevationPin;

	// status LED
	uint8_t m_statusLedPin;

	// constants
	static const uint8_t m_tolerance = 10;
	static const uint8_t m_speedFast = 250;
	static const uint8_t m_speedSlow = 50;
	static const uint8_t m_speedManual = 100;
	static const uint8_t m_speedSwitchover = 20;

};

#endif