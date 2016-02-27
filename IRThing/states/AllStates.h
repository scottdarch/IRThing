/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/


#ifndef ALLSTATES_H_
#define ALLSTATES_H_


#include "State.h"
#include "Pulse.h"

// +--[ RUNNING ]-------------------------------------------------------------+
State* InitRunningState(State* newState, State* parentState, State* substates[], size_t substateCount);

// +--[ VISUALIZE ]-----------------------------------------------------------+
State* InitVisualizeState(State* newState, State* parentState);

// +--[ CAPTURE ]-------------------------------------------------------------+
typedef void (*OnPatternCaptureFunc)(State* captureState, Pulse* pulses, uint8_t pulseCount);
typedef void (*OnPatternCaptureFailedFunc)(State* captureState);

State* InitCaptureState(State* newState, State* parentState, OnPatternCaptureFunc captureCallback, OnPatternCaptureFailedFunc captureFailedCallback);

// +--[ REPEAT ]--------------------------------------------------------------+
State* InitRepeatState(State* newState, State* parentState);

#endif /* ALLSTATES_H_ */
