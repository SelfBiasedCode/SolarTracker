#ifndef AUTO_MODE_H
#define AUTO_MODE_H

#include <LiquidCrystal.h>
#include "../Tracker.hpp"
#include "../InputInfo.hpp"
#include "../OutputInfo.hpp"
#include "Button.hpp"

namespace SolarTracker::States
{
    class AutoMode
    {
    public:
        AutoMode(SolarTracker::Tracker& tracker, LiquidCrystal& lcd) :m_tracker(tracker), m_lcd(lcd) {}

        void init()
        {
            // print static components
            m_lcd.setCursor(0, 0);
            m_lcd.print("AU A:    SPD:");
            m_lcd.setCursor(1, 0);
            m_lcd.print("TO E:    SPD:");
        }

        void process()
        {
            execute();
            display();
        }

        void execute()
        {
            m_tracker.autoAdjust();
        }

        void display()
        {
            // get data
            SolarTracker::OutputInfo outData = m_tracker.getOutputInfo();

            // print azimuth information
            m_lcd.setCursor(0, 5);
            switch (outData.azimuthDirection)
            {
            case SolarTracker::Direction::Positive:
                m_lcd.print("POS");
                break;

            case SolarTracker::Direction::Negative:
                m_lcd.print("NEG");
                break;

            case SolarTracker::Direction::Stop:
                m_lcd.print("OFF");
                break;
            }

            m_lcd.setCursor(0, 13);
            m_lcd.print(outData.azimuthSpeed);

            // print elevation information
            m_lcd.setCursor(1, 5);
            switch (outData.elevationDirection)
            {
            case SolarTracker::Direction::Positive:
                m_lcd.print("POS");
                break;

            case SolarTracker::Direction::Negative:
                m_lcd.print("NEG");
                break;

            case SolarTracker::Direction::Stop:
                m_lcd.print("OFF");
                break;
            }

            m_lcd.setCursor(1, 13);
            m_lcd.print(outData.elevationSpeed);
        }

        // delete constructors
        AutoMode(AutoMode const&) = delete;
        AutoMode& operator=(AutoMode const&) = delete;
        AutoMode() = delete;

    private:
        SolarTracker::Tracker& m_tracker;
        LiquidCrystal& m_lcd;
    };
} // namespace


#endif
