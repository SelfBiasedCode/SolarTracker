#include <LiquidCrystal.h>
#include "SolarTracker.hpp"
#include "Button.hpp"
#include "DisplayText.hpp"

// analog inputs
#define LDR_TOPLEFT_PIN A1
#define LDR_TOPRIGHT_PIN A2
#define LDR_BOTLEFT_PIN A3
#define LDR_BOTRIGHT_PIN A4

// digital outputs
#define MOTOR_AZIMUTH_POS 14
#define MOTOR_AZIMUTH_NEG 15
#define MOTOR_ELEVATION_POS A6
#define MOTOR_ELEVATION_NEG A7

// PWM outputs
#define MOTOR_AZIMUTH_SIG_PIN 2
#define MOTOR_ELEVATION_SIG_PIN 3

// LCD Shield
#define SWITCHES_PIN A0
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define LED_STATUS_PIN 0

#define MODE_SWITCH_INVOCATION_COUNT 10

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

  lcd.begin(16, 2);              // start the library
  
}

Button readButtonInputs()
{
  // read value from ADC
  uint16_t inputValue = analogRead(SWITCHES_PIN);

  // return button based on value
  if (adc_key_in > 1000) return Button.None;
  if (adc_key_in < 50)   return Button.Right;
  if (adc_key_in < 195)  return Button.Up;
  if (adc_key_in < 380)  return Button.Down;
  if (adc_key_in < 555)  return Button.Left;
  if (adc_key_in < 790)  return Button.Select;

  // fail
  return Button.None;
}

void loop()
{
  bool skipMovement = false;

  // get button states
  Button buttonPressed = readButtonInputs();

  lcd.clear();

  // mode selection
  if (autoMode)
  {
    // if auto mode is active, switch to manual if a button was pressed
    autoMode = buttonPressed == None;

    buttonCounter = 0;
    lcd.setCursor(0, 0);
    lcd.print("Mode: Auto");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Manual");

    // auto mode inactive: switch back if left and right buttons are pressed for a certain number of invocations
    if (buttonPressed == Button.Select)
    {
      buttonCounter++;

      // reset button state to prevent movement in the following code
      buttonPressed = Button.None;
      lcd.setCursor(0, 1);
      lcd.print("Hold for Auto...");
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
  if (autoMode && !skipMovement)
  {
    tracker.autoAdjust();
  }
  else if (!skipMovement)
  {
    // East/West
    if (buttonPressed == Button.Left)
    {
      tracker.manualAdjust(SolarTracker::Axis::Azimuth, SolarTracker::Direction::Positive);
      lcd.setCursor(0, 1);
      lcd.print("Azi Pos");
    }
    else if (buttonPressed == Button.Right)
    {
      tracker.manualAdjust(SolarTracker::Axis::Azimuth, SolarTracker::Direction::Negative);
      lcd.setCursor(0, 1);
      lcd.print("Azi Neg");
    }
    else
    {
      tracker.manualAdjust(SolarTracker::Axis::Azimuth, SolarTracker::Direction::Stop);
      lcd.setCursor(0, 1);
      lcd.print("Azi Off");
    }

    // Up/Down
    if (buttonPressed == Button.Up)
    {
      tracker.manualAdjust(SolarTracker::Axis::Elevation, SolarTracker::Direction::Positive);
      lcd.setCursor(9, 1);
      lcd.print("Ele Pos");
    }
    else if (buttonPressed == Button.Down)
    {
      tracker.manualAdjust(SolarTracker::Axis::Elevation, SolarTracker::Direction::Negative);
      lcd.setCursor(9, 1);
      lcd.print("Ele Neg");
    }
    else
    {
      tracker.manualAdjust(SolarTracker::Axis::Elevation, SolarTracker::Direction::Stop);
      lcd.setCursor(9, 1);
      lcd.print("Ele Off");
    }
  }

  // loop throttling
  delay(100);
}
