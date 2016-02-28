/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#ifndef MACHINE_H_
#define MACHINE_H_

#include "tinker/State.h"

struct _MachineType;

typedef void (*OnStateChangeFunc)(struct _MachineType* machine, State* oldState, State* newState);

/**
 * \struct Machine
 * Hierarchical state machine object type.
 */
typedef struct _MachineType
{
    /**
     * Opaque pointer available for external use.
     * This pointer is neither read nor written by the
     * Machine object.
     */
    void* userData;
    void* _data;  
} Machine;

/**
 * Objective-C style object initializer for Machine types.
 * \param  machine              The machine object to initialize.
 * \param  stateChangeHandler   A state change handler method to set
 *                              for the machine object.
 */
Machine* MachineInit(Machine* machine, OnStateChangeFunc stateChangeHandler);

/**
 * Machine de-initializer. This method may de-allocate internal data but will
 * not free the machine pointer (i.e. this is a destructor not a deleter).
 */
void MachineDestructor(Machine* machine);

/**
 * Instruct a machine to move into a given state.
 * \param  machine  The machine to request a state change for.
 * \param  state    The state to move the machine into.
 */
StateErrorType SetMachineState(Machine* machine, State* state);

/**
 * Instruct a machine to move into a given state and pass along opaque data with the state transition.
 * \param  machine  The machine to request a state change for.
 * \param  state    The state to move the machine into.
 * \param  data     The data to provide with the state transition.
 * \param  dataLen  The size in bytes of the data argument.
 */
StateErrorType SetMachineStateWData(Machine* machine, State* state, void* data, uint8_t dataLen);

/**
 * Get the leaf state at the bottom of the current state hierarchy.
 * \param  machine  The machine to get the state from.
 * \return A state or 0 if there is no current state set for the machine.
 */
State* GetMachineFocus(Machine* machine);

#endif /* MACHINE_H_ */
