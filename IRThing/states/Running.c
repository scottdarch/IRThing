/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "states/AllStates.h"

StateErrorType OnEnterRunningState(State* state, void* data, uint8_t datalen)
{
    PORTA |= _BV(PINA_PERIPH);
    return STATE_ERROR_NONE;    
}

StateErrorType OnExitRunningState(State* state, void* data, uint8_t datalen)
{
    PORTA &= ~_BV(PINA_PERIPH);
    return STATE_ERROR_NONE;
}

State* InitRunningState(State* newState, State* parentState, State* substates[], size_t substateCount)
{
    return StateInitWSubstates(newState, parentState, OnEnterRunningState, OnExitRunningState, 0, substates, substateCount);
}
