#include <LiquidCrystal.h>
#include "SolarTracker.hpp"

// analog inputs
#define LDR_TOPLEFT_PIN A1
#define LDR_TOPRIGHT_PIN A2
#define LDR_BOTLEFT_PIN A3
#define LDR_BOTRIGHT_PIN A4

// digital outputs
#define MOTOR_AZIMUTH_POS 14 // red
#define MOTOR_AZIMUTH_NEG 15 // black
#define MOTOR_ELEVATION_POS A8 // white
#define MOTOR_ELEVATION_NEG A9 // orange

// PWM outputs
#define MOTOR_AZIMUTH_SIG_PIN 2 // orange
#define MOTOR_ELEVATION_SIG_PIN 3 // blue

// LCD Shield
#define BUTTONS_PIN A0
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// SolarTracker
using namespace SolarTracker;
static const Config trackerConfig =
{
ldr_topLeftPin: LDR_TOPLEFT_PIN,
ldr_topRightPin: LDR_TOPRIGHT_PIN,
ldr_bottomLeftPin: LDR_BOTLEFT_PIN,
ldr_bottomRightPin: LDR_BOTRIGHT_PIN,
motor_azimuth_signalPin: MOTOR_AZIMUTH_SIG_PIN,
motor_azimuth_positivePin: MOTOR_AZIMUTH_POS,
motor_azimuth_negativePin: MOTOR_AZIMUTH_NEG,
  motor_azimuth_minPwm: 50,
motor_elevation_signalPin: MOTOR_ELEVATION_SIG_PIN,
motor_elevation_positivePin: MOTOR_ELEVATION_POS,
motor_elevation_negativePin: MOTOR_ELEVATION_NEG,
  motor_elevation_minPwm: 60,
};
Tracker tracker(trackerConfig);
States::StateMachine stateMachine(tracker, lcd, BUTTONS_PIN);

// UI
#define DEBUG

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("init()");
#endif
  // initialize Tracker
  tracker.init();

  // initialize LCD
  lcd.begin(16, 2);

  // initizalize State Machine
  stateMachine.init();


}

void loop()
{
  // execute State Machine
  stateMachine.process();

  // loop throttling
  delay(100);
}
