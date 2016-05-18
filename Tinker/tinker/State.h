/*
Copyright 2016 Scott A Dixon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#ifndef STATE_H_
#define STATE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// +--------------------------------------------------------------------------+
// | STATE TYPES
// +--------------------------------------------------------------------------+

struct _StateType;

/**
 * Type all STATE_ERROR_XXXX values can be interpreted as.
 */
typedef int8_t StateErrorType;

#define STATE_ERROR_NONE 0x01
#define STATE_ERROR_FALSE 0x00
#define STATE_ERROR_INVALID -0x01
#define STATE_ERROR_INTERNAL -0x02

/**
 * Type all STATE_INT_XXXX values can be interpreted as.
 */
typedef int8_t StateInterruptType;

// TODO: This isn't well factored. Think though more how to generically
//       classify interrupts and allow states to handle them.
#define STATE_INT_BUTTON_CLICK 0x01

/**
 * Type for any state transition handling functions. See
 * \link State \endlink for specific state transition functions.
 * \param  state    The state in transition.
 * \param  data     Optional, opaque data provided as part of the state
 *                  transition operation. See \link SetMachineStateWData \endlink
 *                  for details. May be 0.
 * \param  datalen  The length in bytes of the data.
 * \return STATE_ERROR_NONE. Returning any other error value will cancel the state
 *         transition.
 */
typedef StateErrorType (*StateTransitionFunc)(struct _StateType* state, void* data, uint8_t datalen);

/**
 * Generic function type for various State methods.
 */
typedef void (*StateFunc)(struct _StateType*);

/**
 * Type for function allowing a state to optionally handle various interrupts.
 * \param  state            The state this message is being dispatched to.
 * \param  interruptType    The type of interrupt currently being processed.
 * \return STATE_ERROR_FALSE to decline to handle the interrupt, STATE_ERROR_NONE
 *         to indicate the interrupt has been handled, or another STATE_ERROR_XXXX
 *         to indicate the state tried but failed to handle the interrupt.
 */
typedef StateErrorType (*StateInterruptFunc)(struct _StateType* state, StateInterruptType interruptType);

/**
 * \struct State
 * A Hierarchical State object type.
 */
typedef struct _StateType
{
    /**
     * Optional interrupt handler. Initialized to 0
     * by State init methods, specializations of State
     * can provide their own to be given a chance to process
     * various interrupts.
     */
    StateInterruptFunc OnInterrupt;

    /**
     * Opaque pointer available for external use.
     * This pointer is neither read nor written by the
     * Machine object nor the State object.
     */
    void* userData;
    StateFunc _OnLoop;
    StateTransitionFunc _OnEnterState;
    StateTransitionFunc _OnExitState;
    void* _storage;
} State;

// +--------------------------------------------------------------------------+
// | STATE METHODS
// +--------------------------------------------------------------------------+

/**
 * Objective-C like State object initializer.
 * \param  newState      The state object to initialize.
 * \param  parentState   The parent state in the state hierarchy. Should not be NULL
 *                       (Root State must be initialized using \link StateInitWSubstates \endlink).
 * \param  enter         An optional transition function invoked when this state
 *                       is entered.
 * \param  exit          An optional transition function invoked when this state
 *                       is exited.
 * \param  onLoop        An optional function to be invoked when this state is entered by a
 *                       run loop.
 * \return A pointer to the initialized state object.
 */
State* StateInit(State* newState, State* parentState, StateTransitionFunc enter, StateTransitionFunc exit, StateFunc onLoop);

/**
 * Objective-C like State object initializer.
 * \param  newState      The state object to initialize.
 * \param  parentState   The parent state in the state hierarchy. May be 0 for the
 *                       root state.
 * \param  enter         An optional transition function invoked when this state
 *                       is entered.
 * \param  exit          An optional transition function invoked when this state
 *                       is exited.
 * \param  onLoop        An optional function to be invoked when this state is entered by a
 *                       run loop.
 * \param  substates     An array of states that are the newState's children.
 * \param  substateCount The number of states in the substates array.
 * \return A pointer to the initialized state object.
 */
State* StateInitWSubstates(State* newState, State* parentState, StateTransitionFunc enter, StateTransitionFunc exit, StateFunc onLoop, State* substates[], size_t substateCount);

/**
 * State de-initializer. This method may de-allocate internal data but will
 * not free the State pointer (i.e. this is a destructor not a deleter).
 */
void StateDestructor(State* state);

/**
 * Query a state to discover it is currently entered. Remember that this state is
 * part of a hierarchy and may not be the bottom-most entered state. Use
 * \link GetMachineFocus \endlink to find such states.
 *
 * \param  state    The state to query.
 * \return true if state is entered else false.
 */
bool StateIsEntered(State* state);

/**
 * Query a state to discover if it is the root state (i.e. it has no parent
 * states).
 * \param  state    The state to query.
 * \return true if state is the root state else false.
 */
bool IsRootState(State* state);

/**
 * Provide a state with an opportunity to process an interrupt.
 * \param  state            The state to invoke an interrupt handler on.
 * \param  interruptType    These are not yet well factored. Currently the only
 *                          valid value is STATE_INT_BUTTON_CLICK.
 * \return If this method returns STATE_ERROR_FALSE the state declined to handle
 *         the interrupt (possibly because it did not have an interrupt handler).
 *         STATE_ERROR_NONE is returned to indicate the interrupt has been handled and
 *         another STATE_ERROR_XXXX is returned to indicate the state tried but failed
 *         to handle the interrupt.
 */
StateErrorType StateHandleInterrupt(State* state, StateInterruptType interruptType);

#endif /* STATE_H_ */
