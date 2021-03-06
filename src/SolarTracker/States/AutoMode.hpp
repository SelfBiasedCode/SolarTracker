#ifndef AUTO_MODE_H
#define AUTO_MODE_H

#include <LiquidCrystal.h>
#include "../Tracker.hpp"
#include "../InputInfo.hpp"
#include "../OutputInfo.hpp"
#include "Button.hpp"
#include "../L298N_Driver.hpp"

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
            m_lcd.setCursor(0, 1);
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
            m_lcd.setCursor(5, 0);
            switch (outData.azimuthCommand)
            {
            case L298N_Driver::Command::Positive:
                m_lcd.print("POS");
                break;

            case L298N_Driver::Command::Negative:
                m_lcd.print("NEG");
                break;

            case L298N_Driver::Command::Brake:
                m_lcd.print("BRK");
                break;

            case L298N_Driver::Command::Off:
                m_lcd.print("OFF");
                break;
            }
            // TODO
            m_lcd.setCursor(13, 0);
            m_lcd.print("   ");
            m_lcd.setCursor(13, 0);
            m_lcd.print(outData.azimuthSpeed);

            // print elevation information
            m_lcd.setCursor(5, 1);
            switch (outData.elevationCommand)
            {
            case L298N_Driver::Command::Positive:
                m_lcd.print("POS");
                break;

            case L298N_Driver::Command::Negative:
                m_lcd.print("NEG");
                break;

            case L298N_Driver::Command::Brake:
                m_lcd.print("BRK");
                break;

            case L298N_Driver::Command::Off:
                m_lcd.print("OFF");
                break;
            }
            m_lcd.setCursor(13, 1);
            m_lcd.print("   ");
            m_lcd.setCursor(13, 1);
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
