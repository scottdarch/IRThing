/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/
#include "states/AllStates.h"

void OnVisualizeLoop(State* state)
{
    if (!(PINA & _BV(PINA_IR_IN)))
    {
        PORTA |= _BV(PINA_VISUAL);
    }
    else
    {
        PORTA &= ~_BV(PINA_VISUAL);
    }
    _delay_us(5);
}

StateErrorType OnEnterVisualizeState(State* state, void* data, uint8_t datalen)
{
    PORTA &= ~_BV(PINA_VISUAL);
    return STATE_ERROR_NONE;
}

StateErrorType OnExitVisualizeState(State* state, void* data, uint8_t datalen)
{
    PORTA &= ~_BV(PINA_VISUAL);
    return STATE_ERROR_NONE;
}

State* InitVisualizeState(State* newState, State* parentState)
{
    newState = StateInit(newState, parentState, OnEnterVisualizeState, OnExitVisualizeState, OnVisualizeLoop);
    if (newState)
    {
                
    }
    return newState;
}
