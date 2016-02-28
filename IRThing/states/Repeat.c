/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "states/AllStates.h"

typedef struct _RepeatData
{
    Pulse* pulses;
    uint8_t pulseCount;
} RepeatData;

extern void OnVisualizeLoop(State* state);

StateErrorType OnEnterRepeatState(State* state, void* data, uint8_t datalen)
{
    PORTA &= ~_BV(PINA_VISUAL);
    RepeatData* repeatData = (RepeatData*)state->userData;
    if (repeatData)
    {
        repeatData->pulses = (Pulse*)data;
        repeatData->pulseCount = datalen;
    }
    return STATE_ERROR_NONE;
}

StateErrorType OnExitRepeatState(State* state, void* data, uint8_t datalen)
{
    PORTA &= ~_BV(PINA_VISUAL);
    return STATE_ERROR_NONE;
}

StateErrorType OnInterruptRepeatState(State* state, StateInterruptType interruptType)
{
    RepeatData* repeatData = (RepeatData*)state->userData;
    if (repeatData)
    {
        for(uint8_t i = 0; i < repeatData->pulseCount; ++i)
        {
            Pulse pulse = repeatData->pulses[i];
            SETPIN_HIGH(A, 1);
            SETPIN_HIGH(A, 2);
            uint16_t pulseHigh = (0x80 & pulse.high) ? ((uint16_t)(0x7f & pulse.high) << 8) : pulse.high;
            for(uint16_t x = 0; x < pulseHigh; ++x)
            {
                _delay_us(22);
            }
            SETPIN_LOW(A, 1);
            SETPIN_LOW(A, 2);
            uint16_t pulseLow =(0x80 & pulse.low) ? ((uint16_t)(0x7f & pulse.low) << 8) : pulse.low;
            // The last low pulse is just the end of the transmission. Don't
            // block on it.
            if (i < repeatData->pulseCount - 1)
            {
                for(uint16_t x = 0; x < pulseLow; ++x)
                {
                    _delay_us(21);
                }
            }
        }
    }
    return STATE_ERROR_NONE;
}

State* InitRepeatState(State* newState, State* parentState)
{
    newState = StateInit(newState, parentState, OnEnterRepeatState, OnExitRepeatState, OnVisualizeLoop);
    if (newState)
    {
        newState->userData = malloc(sizeof(RepeatData));
        newState->OnInterrupt = OnInterruptRepeatState;
    }
    return newState;
}