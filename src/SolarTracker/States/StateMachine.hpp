#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <LiquidCrystal.h>

#include "Button.hpp"
#include "State.hpp"
#include "TestMode.hpp"
#include "ManualMode.hpp"
#include "AutoMode.hpp"
#include "TransitionMode.hpp"
#include "InitMode.hpp"

// TODO: Config?
#define BUTTON_HOLD_CYCLES 30

namespace SolarTracker::States
{
    class StateMachine
    {
    public:
        StateMachine(SolarTracker::Tracker& tracker, LiquidCrystal& lcd, const uint8_t buttonPin) :
            m_testMode(tracker, lcd), m_manualMode(tracker, lcd), m_autoMode(tracker, lcd), m_transMode(tracker, lcd, BUTTON_HOLD_CYCLES), m_initMode(tracker, lcd),
            m_lcd(lcd), m_buttonPin(buttonPin), m_currentState(State::Init)
        {
        }

        void init()
        {
            m_initMode.init();
        }

        Button readButtonState()
        {
            // read value from ADC
            uint16_t inputValue = analogRead(m_buttonPin);

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

        void process()
        {
            Button button = readButtonState();

            State nextState = m_currentState;
            // determine next state
            switch (m_currentState)
            {
            case State::Init:
            {
                // transition after init text
                bool cont = m_initMode.process();
                if (cont)
                {
                    nextState = State::Manual;
                }
                break;
            }

            case State::Manual:
                // check if auto mode is requested
                if (button == Button::Select)
                {
                    m_transMode.init(State::Auto, m_currentState);
                    nextState = State::Transition;
                }
                break;

            case State::Auto:
                // check if manual mode is requested
                if (button == Button::Left || button == Button::Right || button == Button::Up || button == Button::Down)
                {
                    // switch immediately
                    nextState = State::Manual;
                }
                break;

            case State::Transition:
                // Transition mode returns either requested or previous state
                nextState = m_transMode.process(button);
                break;

            default: 
                // TODO: Error?
                break;
            }

            // on change: clear display
            if (nextState != m_currentState)
            {
                m_lcd.clear();
            }

            // run state
            switch (nextState)
            {
            case State::Test:
                m_testMode.process(button);
                break;
            case State::Manual:
                if (nextState != m_currentState) m_manualMode.init();
                m_manualMode.process(button);
                break;
            case State::Auto:
                if (nextState != m_currentState) m_autoMode.init();
                m_autoMode.process();
                break;
            default:
                break;
            }

            m_currentState = nextState;
        }

    private:
        TestMode m_testMode;
        ManualMode m_manualMode;
        AutoMode m_autoMode;
        TransitionMode m_transMode;
        InitMode m_initMode;

        LiquidCrystal& m_lcd;
        const uint8_t m_buttonPin;
        State m_currentState;
    };
} // namespace

#endif
