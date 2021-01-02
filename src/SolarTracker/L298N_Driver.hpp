#ifndef L298N_DRIVER_HPP
#define L298N_DRIVER_HPP

#include "Arduino.h"
#include "ChannelData.hpp"

class L298N_Driver
{
public:
    enum class Channel : uint8_t { Ch1 = 1, Ch2 = 2 };
    enum class Command : uint8_t { Positive, Negative, Brake, Off };
    L298N_Driver(const uint8_t ch1EnablePin, const uint8_t ch1PositivePin, const uint8_t ch1NegativePin, uint8_t const ch2EnablePin, const uint8_t ch2PositivePin, const uint8_t ch2NegativePin)
        : m_ch1En(ch1EnablePin), m_ch1Data(ch1PositivePin, ch1NegativePin), m_ch2En(ch2EnablePin), m_ch2Data(ch2PositivePin, ch2NegativePin)
    {
    }

    void init()
    {
#ifdef L298N_DRIVER_DEBUG
        Serial.print("[L298N] init()");
#endif
        // set direction registers
        pinMode(m_ch1En, OUTPUT);
        pinMode(m_ch2En, OUTPUT);

        // set initial state
        analogWrite(m_ch1En, 0);
        analogWrite(m_ch2En, 0);

        // init channel data
        // get initial states for caching & implicitly set direction registers
        m_ch1Data.getPosState(false);
        m_ch1Data.getNegState(false);
        m_ch2Data.getPosState(false);
        m_ch2Data.getNegState(false);

#ifdef L298N_DRIVER_DEBUG
        Serial.println(" end");
#endif
    }

    bool sense(Channel channel, Command cmd, bool cached = false)
    {
#ifdef L298N_DRIVER_DEBUG
        Serial.print("[L298N] sense()");
#endif
        ChannelData* chanData;

        // this is also used as continuation indicator
        bool result = true;

        // select pins
        switch (channel)
        {
        case Channel::Ch1:
            chanData = &m_ch1Data;
            break;
        case Channel::Ch2:
            chanData = &m_ch2Data;
            break;
        default:
            // unknown channel: unable to handle
            result = false;
        }

        // test
        if (result)
        {
            switch (cmd)
            {
            case Command::Positive:
                result = chanData->getPosState(cached);
                break;

            case Command::Negative:
                result = chanData->getNegState(cached);
                break;

            case Command::Brake:
            case Command::Off:
                // always allowed
                result = true;
                break;

            default:
                // unknown command: unable to handle
                result = false;
                break;
            }
        }

#ifdef L298N_DRIVER_DEBUG
        Serial.print(" Ch ");
        Serial.print((uint8_t)channel);
        Serial.print(" , Cmd ");
        Serial.print((uint8_t)cmd);
        Serial.print(", result ");
        Serial.println(result);
#endif

        return result;
    }

    bool exec(Channel channel, Command cmd, uint8_t dutyCycle = 0)
    {
        // test if the requested action is allowed
        bool allowed = sense(channel, cmd);
        if (!allowed) return false;

        // processing variables
        uint8_t enPin;
        ChannelData* chanData;

        // select pins
        switch (channel)
        {
        case Channel::Ch1:
            enPin = m_ch1En;
            chanData = &m_ch1Data;
            break;
        case Channel::Ch2:
            enPin = m_ch2En;
            chanData = &m_ch2Data;
            break;
        default:
            return false;
        }

        // select command
        switch (cmd)
        {
        case Command::Positive:
            analogWrite(enPin, dutyCycle);
            chanData->setPosState(true);
            chanData->setNegState(false);
            break;

        case Command::Negative:
            analogWrite(enPin, dutyCycle);
            chanData->setPosState(false);
            chanData->setNegState(true);
            break;

        case Command::Brake:
            analogWrite(enPin, dutyCycle);
            chanData->setPosState(true);
            chanData->setNegState(true);
            break;

        case Command::Off:
            analogWrite(enPin, 0);
            chanData->setPosState(false);
            chanData->setNegState(false);
            break;
        default:
            return false;
        }

#ifdef L298N_DRIVER_DEBUG
        Serial.print(": Ch ");
        Serial.print((uint8_t)channel);
        Serial.print(" , Cmd ");
        Serial.print((uint8_t)cmd);
        Serial.print(", dc ");
        Serial.println(dutyCycle);
#endif

        // return positive indication
        return true;
    }

private:
    const uint8_t m_ch1En;
    ChannelData m_ch1Data;
    const uint8_t m_ch2En;
    ChannelData m_ch2Data;
};

#endif

