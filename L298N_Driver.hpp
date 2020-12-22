#ifndef L298N_DRIVER_HPP
#define L298N_DRIVER_HPP

#include "Arduino.h"

#define SENSE_INVERT
// #define L298N_DRIVER_DEBUG
#define READ_DELAY_MS 1

class L298N_Driver
{
public:
    enum class Channel : uint8_t { Ch1, Ch2 };
    enum class Command : uint8_t { Positive, Negative, Brake, Off };
    L298N_Driver(uint8_t ch1EnablePin, uint8_t ch1PositivePin, uint8_t ch1NegativePin, uint8_t ch2EnablePin, uint8_t ch2PositivePin, uint8_t ch2NegativePin)
        : m_ch1En(ch1EnablePin), m_ch1Pos(ch1PositivePin), m_ch1Neg(ch1NegativePin), m_ch2En(ch2EnablePin), m_ch2Pos(ch2PositivePin), m_ch2Neg(ch2NegativePin)
    {
    }

    void init()
    {
#ifdef L298N_DRIVER_DEBUG
        Serial.println("[L298N] init()");
#endif
        // set direction registers
        pinMode(m_ch1En, OUTPUT);
        pinMode(m_ch1Pos, OUTPUT);
        pinMode(m_ch1Neg, OUTPUT);
        pinMode(m_ch2En, OUTPUT);
        pinMode(m_ch2Pos, OUTPUT);
        pinMode(m_ch2Neg, OUTPUT);


        // set initial state
        analogWrite(m_ch1En, 0);
        analogWrite(m_ch2En, 0);
        digitalWrite(m_ch1Pos, LOW);
        digitalWrite(m_ch1Neg, LOW);
        digitalWrite(m_ch2Pos, LOW);
        digitalWrite(m_ch2Neg, LOW);
    }

    bool sense(Channel channel, Command cmd)
    {
        uint8_t positivePin;
        uint8_t negativePin;
        bool result;

        // select pins
        switch (channel)
        {
        case Channel::Ch1:
            positivePin = m_ch1Pos;
            negativePin = m_ch1Neg;
            break;
        case Channel::Ch2:
            positivePin = m_ch2Pos;
            negativePin = m_ch2Neg;
            break;
        }

        // test
        switch (cmd)
        {
        case Command::Positive:
            pinMode(positivePin, INPUT);
            delay(READ_DELAY_MS);
            result = digitalRead(positivePin);
            pinMode(positivePin, OUTPUT);
            break;

        case Command::Negative:
            pinMode(negativePin, INPUT);
            delay(READ_DELAY_MS);
            result = digitalRead(negativePin);
            pinMode(negativePin, OUTPUT);
            break;

        default:
            result = false;
            break;
        }

#ifdef L298N_DRIVER_DEBUG
        Serial.print("[L298N] sense(): Ch ");
        Serial.print((uint8_t) channel);
        Serial.print(" , Cmd ");
        Serial.print((uint8_t) cmd);
        Serial.print(", result ");
        Serial.println(result);
#endif

        return result;
    }

    bool exec(Channel channel, Command cmd, uint8_t dutyCycle = 0)
    {
        // test if the requested action is allowed
        bool allowed = sense(channel, cmd);

#ifdef SENSE_INVERT
        if (allowed) return false;
#else
        if (!allowed) return false;
#endif

        // processing variables
        uint8_t enPin;
        uint8_t positivePin;
        uint8_t negativePin;

        // select pins
        switch (channel)
        {
        case Channel::Ch1:
            enPin = m_ch1En;
            positivePin = m_ch1Pos;
            negativePin = m_ch1Neg;
            break;
        case Channel::Ch2:
            enPin = m_ch2En;
            positivePin = m_ch2Pos;
            negativePin = m_ch2Neg;
            break;
        }

        // select command
        switch (cmd)
        {
        case Command::Positive:
            analogWrite(enPin, dutyCycle);
            digitalWrite(positivePin, HIGH);
            digitalWrite(negativePin, LOW);
            break;

        case Command::Negative:
            analogWrite(enPin, dutyCycle);
            digitalWrite(positivePin, LOW);
            digitalWrite(negativePin, HIGH);
            break;

        case Command::Brake:
            analogWrite(enPin, dutyCycle);
            digitalWrite(positivePin, HIGH);
            digitalWrite(negativePin, HIGH);
            break;

        case Command::Off:
            analogWrite(enPin, 0);
            digitalWrite(positivePin, LOW);
            digitalWrite(negativePin, LOW);
            break;
        }

#ifdef L298N_DRIVER_DEBUG
        Serial.print("[L298N] exec(): Ch ");
        Serial.print((uint8_t) channel);
        Serial.print(" , Cmd ");
        Serial.print((uint8_t) cmd);
        Serial.print(", dc ");
        Serial.println(dutyCycle);
#endif

        // return positive indication
        return true;
    }

private:
    uint8_t m_ch1En;
    uint8_t m_ch1Pos;
    uint8_t m_ch1Neg;
    uint8_t m_ch2En;
    uint8_t m_ch2Pos;
    uint8_t m_ch2Neg;
};

#endif
