#ifndef TRANSITION_MODE_H
#define TRANSITION_MODE_H

#include <LiquidCrystal.h>
#include "../Tracker.hpp"
#include "../InputInfo.hpp"
#include "../OutputInfo.hpp"
#include "Button.hpp"
#include "State.hpp"

namespace SolarTracker::States
{
    class TransitionMode
    {
    public:

        TransitionMode(SolarTracker::Tracker& tracker, LiquidCrystal& lcd, uint8_t buttonHoldCycles) :m_currentButtonCount(0), m_maxButtonCount(buttonHoldCycles), m_tracker(tracker), m_lcd(lcd) {}

        void init(State requestedState, State currentState)
        {          
            // reset and store data
            m_currentButtonCount = 0;
            m_reqState = requestedState;
            m_oldState = currentState;

            // show message
            display();
        }

        State process(Button button)
        {
            // stop all motion - called repeatedly for PWM smoothing
            m_tracker.manualAdjust(SolarTracker::Tracker::Axis::Azimuth, SolarTracker::Direction::Stop);
            m_tracker.manualAdjust(SolarTracker::Tracker::Axis::Elevation, SolarTracker::Direction::Stop);

            // if button is not held anymore, return to previous state
            if (button != Button::Select)
            {
                return m_oldState;
            }

            // button is still held: count
            m_currentButtonCount++;
            if (m_currentButtonCount >= m_maxButtonCount)
            {
                // button was held long enough -> change to requested state
                return m_reqState;
            }
            else
            {
                // button was not held long enough yet -> remain in this state
                return State::Transition;
            }
        }

        void display()
        {
            m_lcd.setCursor(0, 4);
            m_lcd.print("Hold for");
            m_lcd.setCursor(1, 2);
            switch (m_reqState)
            {
            case State::Test:
                m_lcd.print("Test Mode...");
                break;
            case State::Auto:
                m_lcd.print("Auto Mode...");
                break;
            default:
                m_lcd.print("ERROR!");
                break;
            }
        }

        // delete constructors
        TransitionMode(TransitionMode const&) = delete;
        TransitionMode& operator=(TransitionMode const&) = delete;
        TransitionMode() = delete;

    private:
        uint8_t m_currentButtonCount;
        const uint8_t m_maxButtonCount;
        SolarTracker::Tracker& m_tracker;
        LiquidCrystal& m_lcd;
        
        State m_reqState;
        State m_oldState;
    };
} // namespace


#endif
