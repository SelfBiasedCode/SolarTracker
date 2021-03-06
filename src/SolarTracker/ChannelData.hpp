#ifndef CHANNEL_DATA_HPP
#define CHANNEL_DATA_HPP

#include <stdint.h>
#include "Arduino.h"

// uncomment to use optimized code
//#define USE_ARDUINO

#define INVERT_POLARITY

class ChannelData
{
public:
    ChannelData(const uint8_t& positivePin, const uint8_t& negativePin)
    {
#ifdef USE_ARDUINO
        m_posPin = positivePin;
        m_negPin = negativePin;
#else
        // cache low level data from wiring
        m_posMask = digitalPinToBitMask(positivePin);
        uint8_t port = digitalPinToPort(positivePin);

        m_posInReg = portInputRegister(port);
        m_posOutReg = portOutputRegister(port);
        m_posDirReg = portModeRegister(port);

        m_negMask = digitalPinToBitMask(negativePin);
        port = digitalPinToPort(negativePin);

        m_negInReg = portInputRegister(port);
        m_negOutReg = portOutputRegister(port);
        m_negDirReg = portModeRegister(port);
#endif
    }

    // set pin to input, read, reset pin to output
    bool getPosState(bool cached)
    {
        if (cached)
        {
            return m_posCache;
        }

        bool result;

#ifdef USE_ARDUINO
        // set to input
        pinMode(m_posPin, INPUT);

        // read
        result = digitalRead(m_posPin);

        // set to output
        pinMode(m_posPin, OUTPUT);
#else
        // set to input
        setDir(m_posDirReg, m_posOutReg, m_posMask, true);

        // read
        _NOP();
        _NOP();
        _NOP();
        if (*m_posInReg & m_posMask)
        {
            result = true;
        }
        else
        {
            result = false;
        }

        // set to output
        setDir(m_posDirReg, m_posOutReg, m_posMask, false);
#endif

#ifdef INVERT_POLARITY
// invert polarity if required
result = !result;
#endif
        // cache and return
        m_posCache = result;
        return result;
    }

    bool getNegState(bool cached)
    {
        if (cached)
        {
            return m_negCache;
        }

        bool result;

#ifdef USE_ARDUINO
        // set to input
        pinMode(m_negPin, INPUT);

        // read
        result = digitalRead(m_negPin);

        // set to output
        pinMode(m_negPin, OUTPUT);
#else
        // set to input
        setDir(m_negDirReg, m_negOutReg, m_negMask, true);

        // read
         _NOP();
         _NOP();
         _NOP();
        if (*m_negInReg & m_negMask)
        {
            result = true;
        }
        else
        {
            result = false;
        }

        // set to output
        setDir(m_negDirReg, m_negOutReg, m_negMask, false);
#endif

#ifdef INVERT_POLARITY
// invert polarity if required
result = !result;
#endif
       
        // cache and return
        m_negCache = result;
        return result;
    }

    void setPosState(bool state)
    {
#ifdef USE_ARDUINO
        digitalWrite(m_posPin, state);
#else
        if (state == false)
        {
            *m_posOutReg &= ~m_posMask;
        }
        else
        {
            *m_posOutReg |= m_posMask;
        }
#endif
    }

    void setNegState(bool state)
    {
#ifdef USE_ARDUINO
        digitalWrite(m_negPin, state);
#else
        if (state == false)
        {
            *m_negOutReg &= ~m_negMask;
        }
        else
        {
            *m_negOutReg |= m_negMask;
        }
#endif
    }

private:

    inline void setDir(volatile uint8_t* dirReg, volatile uint8_t* outReg, const uint8_t& bitMask, bool input) const
    {
        if (input)
        {
            *dirReg &= ~bitMask;
            *outReg &= ~bitMask;
        }
        else
        {
            *dirReg |= bitMask;
        };
    }

    

#ifdef USE_ARDUINO
    uint8_t m_posPin;
#else
    uint8_t m_posMask;
    volatile uint8_t* m_posInReg;
    volatile uint8_t* m_posOutReg;
    volatile uint8_t* m_posDirReg;
#endif

#ifdef USE_ARDUINO
    uint8_t m_negPin;
#else
    uint8_t m_negMask;
    volatile uint8_t* m_negInReg;
    volatile uint8_t* m_negOutReg;
    volatile uint8_t* m_negDirReg;
#endif

    // Caching
    bool m_posCache;
    bool m_negCache;
};

#endif
