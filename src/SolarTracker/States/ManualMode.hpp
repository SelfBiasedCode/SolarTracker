#ifndef MANUAL_MODE_H
#define MANUAL_MODE_H

#include <LiquidCrystal.h>
#include "../Tracker.hpp"
#include "../InputInfo.hpp"
#include "Button.hpp"

namespace SolarTracker::States
{
    class ManualMode
    {
    public:
        ManualMode(SolarTracker::Tracker& tracker, LiquidCrystal& lcd) :m_tracker(tracker), m_lcd(lcd) {}

        void init()
        {
            // print static components
            m_lcd.setCursor(0, 0);
            m_lcd.print("A:");
            m_lcd.setCursor(0, 1);
            m_lcd.print("E:");
        }

        void process(Button button)
        {
            execute(button);
            display();
        }

        void execute(Button button)
        {
            SolarTracker::Direction aziDirection;
            SolarTracker::Direction eleDirection;
            switch (button)
            {
            case Button::Left:
                aziDirection = SolarTracker::Direction::Positive;
                break;
            case Button::Right:
                aziDirection = SolarTracker::Direction::Negative;
                break;
            default:
                aziDirection = SolarTracker::Direction::Stop;
                break;
            }

            // Up/Down
            switch (button)
            {
            case Button::Up:
                eleDirection = SolarTracker::Direction::Positive;
                break;
            case Button::Down:
                eleDirection = SolarTracker::Direction::Negative;
                break;
            default:
                eleDirection = SolarTracker::Direction::Stop;
                break;
            }

            // execute manual movement
            m_tracker.manualAdjust(Tracker::Axis::Azimuth, aziDirection);
            m_tracker.manualAdjust(Tracker::Axis::Elevation, eleDirection);
        }

        void display()
        {
            // get data
            SolarTracker::InputInfo inData = m_tracker.getInputInfo();

            // print azimuth information
            m_lcd.setCursor(2, 0);
            
            if (inData.limitSwAziPos)
            {
                m_lcd.print("Limit+");
            }
            else if (inData.limitSwAziNeg)
            {
                m_lcd.print("Limit-");
            }
            else
            {
                m_lcd.print("Free   ");
            }
            

            m_lcd.setCursor(10, 0);
            m_lcd.print("     ");
            m_lcd.setCursor(10, 0);
            m_lcd.print(inData.errorAzimuth);

            // print elevation information
            m_lcd.setCursor(2, 1);
            if (inData.limitSwElePos)
            {
                m_lcd.print("Limit+");
            }
            else if (inData.limitSwEleNeg)
            {
                m_lcd.print("Limit-");
            }
            else
            {
                m_lcd.print("Free   ");
            }

            m_lcd.setCursor(10, 1);
            m_lcd.print("     ");
            m_lcd.setCursor(10, 1);
            m_lcd.print(inData.errorElevation);
        }

        // delete constructors
        ManualMode(ManualMode const&) = delete;
        ManualMode& operator=(ManualMode const&) = delete;
        ManualMode() = delete;

    private:
        SolarTracker::Tracker& m_tracker;
        LiquidCrystal& m_lcd;
    };
} // namespace


#endif
