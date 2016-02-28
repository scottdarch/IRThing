/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#ifndef STATE_H_
#define STATE_H_

#include <stdint.h>
#include <stdlib.h>

// +--------------------------------------------------------------------------+
// | STATE TYPES
// +--------------------------------------------------------------------------+

struct _StateType;

/**
 * Type all STATE_ERROR_XXXX can be interpreted as.
 */
typedef int8_t StateErrorType;

/**
 * Type all 
 */
typedef int8_t StateInterruptType;

#define STATE_ERROR_NONE 0x01
#define STATE_ERROR_FALSE 0x00
#define STATE_ERROR_INVALID -0x01
#define STATE_ERROR_INTERNAL -0x02

#define STATE_INT_BUTTON_CLICK 0x01

typedef StateErrorType (*StateTransitionFunc)(struct _StateType*, void* data, uint8_t datalen);
typedef void (*StateFunc)(struct _StateType*);
typedef StateErrorType (*StateInterruptFunc)(struct _StateType*, StateInterruptType interruptType);

typedef struct _StateType
{
    StateTransitionFunc OnEnterState;
    StateTransitionFunc OnExitState;
    StateFunc OnLoop;
    StateInterruptFunc OnInterrupt;
    void* userData;
    void* _storage;
} State;

// +--------------------------------------------------------------------------+
// | STATE METHODS
// +--------------------------------------------------------------------------+

State* StateInit(State* newState, State* parentState, StateTransitionFunc enter, StateTransitionFunc exit, StateFunc onLoop);
State* StateInitWSubstates(State* newState, State* parentState, StateTransitionFunc enter, StateTransitionFunc exit, StateFunc onLoop, State* substates[], size_t substateCount);

void StateDestructor(State* state);

StateErrorType StateEnter(State* state, void* data, uint8_t datalen);
StateErrorType StateExit(State* state, void* data, uint8_t datalen);
int8_t StateIsEntered(State* state);
int8_t StateIsChildOf(State* state, State* possibleAncestor);
State* GetParentState(State* state);
int8_t IsRootState(State* state);

StateErrorType StateHandleInterrupt(State* state, StateInterruptType interruptType);

#endif /* STATE_H_ */
