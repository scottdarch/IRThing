/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#ifndef MACHINE_H_
#define MACHINE_H_

#include "State.h"

struct _MachineType;

typedef void (*OnStateChangeFunc)(struct _MachineType* machine, State* oldState, State* newState);


typedef struct _MachineType
{
    void* userData;
    void* _data;  
} Machine;

Machine* MachineInit(Machine* machine, OnStateChangeFunc stateChangeHandler);

void MachineDestructor(Machine* machine);

StateErrorType SetMachineState(Machine* machine, State* state);

StateErrorType SetMachineStateWData(Machine* machine, State* state, void* data, uint8_t dataLen);

State* GetMachineFocus(Machine* machine);

#endif /* MACHINE_H_ */
