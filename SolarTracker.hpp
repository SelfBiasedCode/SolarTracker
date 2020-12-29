#ifndef SOLAR_TRACKER_H
#define SOLAR_TRACKER_H

// this is the main include file for SolarTracker.

// debug messages: uncomment to enable
//#define SOLAR_TRACKER_DEBUG
//#define L298N_DRIVER_DEBUG

// convenience includes (relative directory since the Arduino include path cannot be modified)
#include "src/SolarTracker/AxisData.hpp"
#include "src/SolarTracker/Config.hpp"
#include "src/SolarTracker/Direction.hpp"
#include "src/SolarTracker/InputInfo.hpp"
#include "src/SolarTracker/OutputInfo.hpp"
#include "src/SolarTracker/Tracker.hpp"
#include "src/SolarTracker/States/StateMachine.hpp"

#endif
