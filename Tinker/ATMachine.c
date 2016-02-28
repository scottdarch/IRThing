/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/
/**
 * \file ATMachine.c
 * Atmel AVR specific implementation of Machine.h.
 */
#include "tinker/Machine.h"
#include <stdlib.h>
#include <avr/interrupt.h>

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

