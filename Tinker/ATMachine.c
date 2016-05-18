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

/**
 * \file ATMachine.c
 * Atmel AVR specific implementation of Machine.h.
 */
#include "tinker/Machine.h"
#include <stdlib.h>
#include <avr/interrupt.h>

// +--------------------------------------------------------------------------+
// | MACHINE->STATE INTERNAL INTERFACE
// +--------------------------------------------------------------------------+
extern StateErrorType StateEnter(State* state, void* data, uint8_t datalen);
extern StateErrorType StateExit(State* state, void* data, uint8_t datalen);
extern int8_t StateIsChildOf(State* state, State* possibleAncestor);
extern State* GetParentState(State* state);

typedef struct _MachinePrivate
{
    State* focus;
    OnStateChangeFunc stateChangeHandler;
} MachinePrivate;

Machine* MachineInit(Machine* machine, OnStateChangeFunc stateChangeHandler)
{
    if (machine)
    {
        MachinePrivate* private = malloc(sizeof(MachinePrivate));
        private->focus = 0;
        private->stateChangeHandler = stateChangeHandler;
        machine->_data = private;
    }
    return machine;
}

void MachineDestructor(Machine* machine)
{
    if (machine)
    {
        free(machine->_data);
        machine->_data = 0;
    }
}

State* GetMachineFocus(Machine* machine)
{
    State* result = 0;
    if (machine)
    {
        MachinePrivate* private = (MachinePrivate*)machine->_data;
        result = private->focus;
    }
    return result;
}

StateErrorType SetMachineState(Machine* machine, State* state)
{
    return SetMachineStateWData(machine, state, 0, 0);
}

StateErrorType SetMachineStateWData(Machine* machine, State* state, void* data, uint8_t dataLen)
{
    // TODO: remove Atmel specific calls. Make this machine generic.
    cli();
    StateErrorType result = STATE_ERROR_NONE;
    if (machine)
    {
        MachinePrivate* private = (MachinePrivate*)machine->_data;

        if (private->focus != state)
        {
            State* oldState = private->focus;
            State* exitState = 0;
            State* possibleExitState = private->focus;
            while(possibleExitState && !StateIsChildOf(state, possibleExitState) && !IsRootState(possibleExitState))
            {
                exitState = possibleExitState;
                possibleExitState = GetParentState(exitState);
            }

            result = StateExit(exitState, 0, 0);

            asm("nop");

            if (STATE_ERROR_NONE == result && STATE_ERROR_NONE == (result = StateEnter(state, data, dataLen)))
            {
                private->focus = state;
                if (private->stateChangeHandler)
                {
                    private->stateChangeHandler(machine, oldState, state);
                }
            }
        }
    }
    sei();
    return result;
}
