#include "SolarTracker.hpp"

// digital inputs
#define BUTTON_LEFT_PIN 4
#define BUTTON_RIGHT_PIN 7
#define BUTTON_UP_PIN 8
#define BUTTON_DOWN_PIN 9

// analog inputs
#define LDR_TOPLEFT_PIN A0
#define LDR_TOPRIGHT_PIN A1
#define LDR_BOTLEFT_PIN A2
#define LDR_BOTRIGHT_PIN A3

// digital outputs
#define MOTOR_AZIMUTH_POS 2
#define MOTOR_AZIMUTH_NEG 3
#define MOTOR_ELEVATION_POS A6
#define MOTOR_ELEVATION_NEG A7

// PWM outputs
#define MOTOR_AZIMUTH_SIG_PIN 5
#define MOTOR_ELEVATION_SIG_PIN 6


#define LED_STATUS_PIN 0

#define MODE_SWITCH_INVOCATION_COUNT 50

/* SolarTracker */
static const SolarTrackerConfig trackerConfig =
{
ldr_topLeftPin: LDR_TOPLEFT_PIN,
ldr_topRightPin: LDR_TOPRIGHT_PIN,
ldr_bottomLeftPin: LDR_BOTLEFT_PIN,
ldr_bottomRightPin: LDR_BOTRIGHT_PIN,
motor_azimuth_signalPin: MOTOR_AZIMUTH_SIG_PIN,
motor_azimuth_positivePin: MOTOR_AZIMUTH_POS,
motor_azimuth_negativePin: MOTOR_AZIMUTH_NEG,
motor_elevation_signalPin: MOTOR_ELEVATION_SIG_PIN,
motor_elevation_positivePin: MOTOR_ELEVATION_POS,
motor_elevation_negativePin: MOTOR_ELEVATION_NEG,
led_status_pin: LED_STATUS_PIN
};

SolarTracker tracker(trackerConfig);

uint8_t buttonCounter;
bool autoMode;

void setup()
{
  buttonCounter = 0;
  autoMode = true;
}

void loop()
{
  // get button states
  bool leftPressed = digitalRead(BUTTON_LEFT_PIN);
  bool rightPressed = digitalRead(BUTTON_RIGHT_PIN);
  bool upPressed = digitalRead(BUTTON_UP_PIN);
  bool downPressed = digitalRead(BUTTON_DOWN_PIN);

  // mode selection
  if (autoMode)
  {
    // if auto mode is active, switch to manual if a button was pressed
    autoMode = !(leftPressed | rightPressed | upPressed | downPressed);
    buttonCounter = 0;
  }
  else
  {
    // auto mode inactive: switch back if left and right buttons are pressed for a certain number of invocations
    if (leftPressed && rightPressed)
    {
      buttonCounter++;

      // reset button state to prevent movement in the following code
      leftPressed = false;
      rightPressed = false;
    }
    else
    {
      buttonCounter = 0;
    }

    // if buttons were held long enough, switch to auto mode
    if (buttonCounter > MODE_SWITCH_INVOCATION_COUNT)
    {
      autoMode = true;
    }
  }

  // movement
  if (autoMode)
  {
    tracker.autoAdjust();
  }
  else
  {
    // East/West
    if (leftPressed)
    {
      tracker.manualAdjust(SolarTracker::Axis::Azimuth, SolarTracker::Direction::Positive);
    }
    else if (rightPressed)
    {
      tracker.manualAdjust(SolarTracker::Axis::Azimuth, SolarTracker::Direction::Negative);
    }
    else
    {
      tracker.manualAdjust(SolarTracker::Axis::Azimuth, SolarTracker::Direction::Stop);
    }

    // Up/Down
    if (upPressed)
    {
      tracker.manualAdjust(SolarTracker::Axis::Elevation, SolarTracker::Direction::Positive);
    }
    else if (downPressed)
    {
      tracker.manualAdjust(SolarTracker::Axis::Elevation, SolarTracker::Direction::Negative);
    }
    else
    {
      tracker.manualAdjust(SolarTracker::Axis::Elevation, SolarTracker::Direction::Stop);
    }
  }

  // loop throttling
  delay(100);
}
