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
#define MOTOR_AZIMUTH_POS 14 // red
#define MOTOR_AZIMUTH_NEG 15 // black
#define MOTOR_ELEVATION_POS A8 // white
#define MOTOR_ELEVATION_NEG A9 // orange

// PWM outputs
#define MOTOR_AZIMUTH_SIG_PIN 2 // orange
#define MOTOR_ELEVATION_SIG_PIN 3 // blue

// LCD Shield
#define SWITCHES_PIN A0
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// SolarTracker
using namespace SolarTracker;
static const SolarConfig trackerConfig =
{
ldr_topLeftPin: LDR_TOPLEFT_PIN,
ldr_topRightPin: LDR_TOPRIGHT_PIN,
ldr_bottomLeftPin: LDR_BOTLEFT_PIN,
ldr_bottomRightPin: LDR_BOTRIGHT_PIN,
motor_azimuth_signalPin: MOTOR_AZIMUTH_SIG_PIN,
motor_azimuth_positivePin: MOTOR_AZIMUTH_POS,
motor_azimuth_negativePin: MOTOR_AZIMUTH_NEG,
motor_azimuth_minPwm: 40,
motor_elevation_signalPin: MOTOR_ELEVATION_SIG_PIN,
motor_elevation_positivePin: MOTOR_ELEVATION_POS,
motor_elevation_negativePin: MOTOR_ELEVATION_NEG,
motor_elevation_minPwm: 40,
};
Tracker tracker(trackerConfig);

// UI
uint8_t buttonCounter;
bool autoMode;
#define MODE_SWITCH_INVOCATION_COUNT 10
#define TEST_MODE // comment this out to enable auto mode
#define DEBUG

void setup()
{
  buttonCounter = 0;
  autoMode = false;

  // initialize tracker
  tracker.init();

  // initialize lcd
  lcd.begin(16, 2);
  
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
}

Button readButtonInputs()
{
  // read value from ADC
  uint16_t inputValue = analogRead(SWITCHES_PIN);

  // return button based on value
  if (inputValue > 1000) return Button::None;
  if (inputValue < 50)   return Button::Right;
  if (inputValue < 195)  return Button::Up;
  if (inputValue < 380)  return Button::Down;
  if (inputValue < 555)  return Button::Left;
  if (inputValue < 790)  return Button::Select;

  // fail
  return Button::None;
}

void loop()
{
  // UI variables
  bool skipMovement = false;
  DisplayLine1 line1 = DisplayLine1::Auto;
  DisplayLine2Full line2 = DisplayLine2Full::None;
  Tracker::Direction aziDirection = Tracker::Direction::Stop;
  Tracker::Direction eleDirection = Tracker::Direction::Stop;

  // get button states
  Button buttonPressed = readButtonInputs();

  // mode selection
  if (autoMode)
  {
    // if auto mode is active, switch to manual if a button was pressed
    autoMode = !(buttonPressed == Button::Left || buttonPressed == Button::Right || buttonPressed == Button::Up || buttonPressed == Button::Down);

    buttonCounter = 0;
    line1 = DisplayLine1::Auto;
  }
  else
  {
    line1 = DisplayLine1::Manual;

    // auto mode inactive: switch back if left and right buttons are pressed for a certain number of invocations
    if (buttonPressed == Button::Select)
    {
      buttonCounter++;

      // prevent movement and inform user
      skipMovement = true;
      line2 = DisplayLine2Full::HoldForAuto;
    }
    else
    {
      buttonCounter = 0;
    }

    // if buttons were held long enough, switch to auto mode
    if (buttonCounter > MODE_SWITCH_INVOCATION_COUNT)
    {
      autoMode = true;
      skipMovement = false; // TODO: Optimize the use of this variable
      #ifdef TEST_MODE
      lcd.clear();
      #endif
    }
  }

  // movement
  if (autoMode && !skipMovement)
  {
#ifdef TEST_MODE
    printTestMode();
    line1 = DisplayLine1::Empty;
    line2 = DisplayLine2Full::Empty;
    delay(400);
#else
    tracker.autoAdjust(); // TODO: Add feedback
#endif

  }
  else if (!skipMovement)
  {
    // East/West
    switch (buttonPressed)
    {
      case Button::Left:
        aziDirection = Tracker::Direction::Positive;
        break;
      case Button::Right:
        aziDirection = Tracker::Direction::Negative;
        break;
      default:
        aziDirection = Tracker::Direction::Stop;
        break;
    }

    // Up/Down
    switch (buttonPressed)
    {
      case Button::Up:
        eleDirection = Tracker::Direction::Positive;
        break;
      case Button::Down:
        eleDirection = Tracker::Direction::Negative;
        break;
      default:
        eleDirection = Tracker::Direction::Stop;
        break;
    }

    // execute manual movement
    tracker.manualAdjust(Tracker::Axis::Azimuth, aziDirection);
    tracker.manualAdjust(Tracker::Axis::Elevation, eleDirection);
  }

  // display output

  // write line 1 with all 16 chars
  lcd.setCursor(0, 0);
  switch (line1)
  {
    case DisplayLine1::Auto:
      lcd.print("Mode: Auto      ");
      break;

    case DisplayLine1::Manual:
      lcd.print("Mode: Manual    ");
      break;

    // empty case: do nothing
    case DisplayLine1::Empty:
      break;
  }

  switch (line2)
  {
    // special case: No split line
    case DisplayLine2Full::HoldForAuto:
      lcd.setCursor(0, 1);
      lcd.print("Hold for Auto...");
      break;

    // empty case: do nothing
    case DisplayLine2Full::Empty:
      break;

    // standard case: Split line
    default:
      printLine2(aziDirection, eleDirection);
      break;
  }

  // loop throttling
  delay(100);
}


void printLine2(Tracker::Direction aziDir, Tracker::Direction eleDir)
{
  lcd.setCursor(0, 1);
  lcd.print("Azi "); // 4
  lcd.setCursor(4, 1); // TODO: Can this be removed?
  printDirectionToString(aziDir); // 3
  lcd.print(" "); // 1
  lcd.setCursor(8, 1); // TODO: Can this be removed?
  lcd.print("Ele "); // 4
  lcd.setCursor(12, 1); // TODO: Can this be removed?
  printDirectionToString(eleDir); // 3
}

void printDirectionToString(Tracker::Direction dir)
{
  switch (dir)
  {
    case Tracker::Direction::Positive:
      lcd.print("Pos ");
      break;
    case Tracker::Direction::Negative:
      lcd.print("Neg ");
      break;
    case Tracker::Direction::Stop:
      lcd.print("Off ");
      break;
  }
}

void printTestMode()
{
  // get data
  SolarTracker::InputInfo info = tracker.getInputInfo();

  // print limit switch data
  lcd.setCursor(1, 0);
  lcd.print(info.limitSwElePos);
  lcd.setCursor(1, 1);
  lcd.print(info.limitSwEleNeg);
  lcd.setCursor(0, 1);
  lcd.print(info.limitSwAziNeg);
  lcd.setCursor(2, 1);
  lcd.print(info.limitSwAziPos);

  // print LDR values
  lcd.setCursor(7, 0);
  if (info.ldrValTopLeft < 1000) lcd.print(" ");
  lcd.print(info.ldrValTopLeft);
  lcd.setCursor(12, 0);
  if (info.ldrValTopRight < 1000) lcd.print(" ");
  lcd.print(info.ldrValTopRight);
  lcd.setCursor(7, 1);
  if (info.ldrValBotLeft < 1000) lcd.print(" ");
  lcd.print(info.ldrValBotLeft);
  lcd.setCursor(12, 1);
  if (info.ldrValBotRight < 1000) lcd.print(" ");
  lcd.print(info.ldrValBotRight);
}
