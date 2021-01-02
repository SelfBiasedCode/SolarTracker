#ifndef INIT_MODE_H
#define INIT_MODE_H

#include <LiquidCrystal.h>
#include "../Tracker.hpp"
#include "../InputInfo.hpp"
#include "../OutputInfo.hpp"
#include "Button.hpp"

#define INIT_CYCLES 30

namespace SolarTracker::States
{
    class InitMode
    {
    public:
        InitMode(SolarTracker::Tracker& tracker, LiquidCrystal& lcd) :m_tracker(tracker), m_lcd(lcd), m_currCycles(0) {}

        void init()
        {
            // print boot text
            m_lcd.setCursor(5, 0);
            m_lcd.print("Solar");
            m_lcd.setCursor(4, 1);
            m_lcd.print("Tracker");
        }

        bool process()
        {
            m_currCycles++;
            if (m_currCycles >= INIT_CYCLES)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        // delete constructors
        InitMode(InitMode const&) = delete;
        InitMode& operator=(InitMode const&) = delete;
        InitMode() = delete;

    private:
        SolarTracker::Tracker& m_tracker;
        LiquidCrystal& m_lcd;

        uint8_t m_currCycles;
    };
} // namespace


#endif
