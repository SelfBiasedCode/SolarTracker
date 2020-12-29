#ifndef TEST_MODE_H
#define TEST_MODE_H

#include <LiquidCrystal.h>
#include "../Tracker.hpp"
#include "../InputInfo.hpp"
#include "Button.hpp"

namespace SolarTracker::States
{
    class TestMode
    {
    public:
        TestMode(SolarTracker::Tracker& tracker, LiquidCrystal& lcd) :m_tracker(tracker), m_lcd(lcd) {}

        void process(Button button)
        {
            execute(button);
            display();
        }

        void execute(Button button)
        {
           
        }

        void display()
        {
            // get data
            SolarTracker::InputInfo inData = m_tracker.getInputInfo();

            // print limit switch data
            m_lcd.setCursor(1, 0);
            m_lcd.print(inData.limitSwElePos);
            m_lcd.setCursor(1, 1);
            m_lcd.print(inData.limitSwEleNeg);
            m_lcd.setCursor(0, 1);
            m_lcd.print(inData.limitSwAziNeg);
            m_lcd.setCursor(2, 1);
            m_lcd.print(inData.limitSwAziPos);

            // print LDR values
            m_lcd.setCursor(7, 0);
            if (inData.ldrValTopLeft < 1000) m_lcd.print(" ");
            m_lcd.print(inData.ldrValTopLeft);
            m_lcd.setCursor(12, 0);
            if (inData.ldrValTopRight < 1000) m_lcd.print(" ");
            m_lcd.print(inData.ldrValTopRight);
            m_lcd.setCursor(7, 1);
            if (inData.ldrValBotLeft < 1000) m_lcd.print(" ");
            m_lcd.print(inData.ldrValBotLeft);
            m_lcd.setCursor(12, 1);
            if (inData.ldrValBotRight < 1000) m_lcd.print(" ");
            m_lcd.print(inData.ldrValBotRight);
        }

        // delete constructors
        TestMode(TestMode const&) = delete;
        TestMode& operator=(TestMode const&) = delete;
        TestMode() = delete;

    private:
        SolarTracker::Tracker& m_tracker;
        LiquidCrystal& m_lcd;
    };
} // namespace

#endif
