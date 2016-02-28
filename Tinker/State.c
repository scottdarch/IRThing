/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "tinker/State.h"

// +--------------------------------------------------------------------------+
// | PRIVATE STATE TYPE
// +--------------------------------------------------------------------------+
typedef struct _StatePrivateType
{
    State* parent;
    uint8_t isEntered;
    StateFunc localOnLoop;
    size_t childStateCount;
    State* childStates[];
} StatePrivate;

StatePrivate* StatePrivateInitWSubstates(StatePrivate* newStatePrivate, State* parentState, StateFunc onLoop, State* substates[], size_t substateCount)
{
    if (newStatePrivate)
    {
        newStatePrivate->parent = parentState;
        newStatePrivate->localOnLoop = onLoop;
        newStatePrivate->isEntered = 0;
        newStatePrivate->childStateCount = substateCount;
        for(size_t i = 0; i < substateCount; ++i)
        {
            newStatePrivate->childStates[i] = substates[i];
        }
    }
    return newStatePrivate;
}

StatePrivate* NewStatePrivate(size_t substateCount)
{
    static const size_t statePrivateSize = sizeof(StatePrivate);
    StatePrivate* priv;
    priv = malloc(statePrivateSize + (sizeof(State*) * substateCount));
    return priv;
}

void StatePrivateDelete(StatePrivate* priv)
{
    free(priv);
}

void BubblingOnLoop(State* state)
{
    if (state)
    {
        StatePrivate* priv = (StatePrivate*)state->_storage;
        if (priv->localOnLoop)
        {
            priv->localOnLoop(state);
        }
        BubblingOnLoop(priv->parent);
    }
}

// +--------------------------------------------------------------------------+
// | STATE METHOD IMPLEMENTATION
// +--------------------------------------------------------------------------+
State* StateInit(State* newState, State* parentState, StateTransitionFunc enter, StateTransitionFunc exit, StateFunc onLoop)
{
    return StateInitWSubstates(newState, parentState, enter, exit, onLoop, 0, 0);
}

State* StateInitWSubstates(State* newState, State* parentState, StateTransitionFunc enter, StateTransitionFunc exit, StateFunc onLoop, State* substates[], size_t substateCount)
{
    if (newState)
    {
        newState->OnEnterState = enter;
        newState->OnExitState = exit;
        newState->OnInterrupt = 0;
        newState->OnLoop = BubblingOnLoop;
        newState->_storage = StatePrivateInitWSubstates(NewStatePrivate(substateCount), parentState, onLoop, substates, substateCount);
    }
    return newState;
}

void StateDestructor(State* state)
{
    if (state)
    {
        StatePrivateDelete((StatePrivate*)state->_storage);
    }
}

StateErrorType StateEnter(State* state, void* data, uint8_t datalen)
{
    StateErrorType result = STATE_ERROR_NONE;
    if (state)
    {
        StatePrivate* priv = (StatePrivate*)state->_storage;
        if (!priv->isEntered)
        {
            if (priv->parent)
            {
                result = StateEnter(priv->parent, data, datalen);
            }
            if (STATE_ERROR_NONE == result)
            {
                result = (state->OnEnterState) ? state->OnEnterState(state, data, datalen) : STATE_ERROR_NONE;
                priv->isEntered = (STATE_ERROR_NONE == result);
            }
        }
    }
    return result;
}

StateErrorType StateExit(State* state, void* data, uint8_t datalen)
{
    StateErrorType result = STATE_ERROR_NONE;
    if (state)
    {
        StatePrivate* priv = (StatePrivate*)state->_storage;
        if (priv->isEntered)
        {
            priv->isEntered = 0;
            for(size_t i = 0; i < priv->childStateCount; ++i)
            {
                result = StateExit(priv->childStates[i], data, datalen);
                if (STATE_ERROR_NONE != result)
                {
                    break;
                }
            }
            if (STATE_ERROR_NONE == result)
            result = (state->OnExitState) ? state->OnExitState(state, data, datalen) : STATE_ERROR_NONE;
        }
    }
    return result;
}

int8_t StateIsEntered(State* state)
{
    int8_t isEntered = 0;
    if (state) {
        StatePrivate* priv = (StatePrivate*)state->_storage;
        isEntered = priv->isEntered;
    }
    return isEntered;
}

StateErrorType StateHandleInterrupt(State* state, StateInterruptType interruptType)
{
    if (state && state->OnInterrupt)
    {
        return state->OnInterrupt(state, interruptType);
    }
    return STATE_ERROR_FALSE;
}

int8_t IsRootState(State* state)
{
    int8_t result = 0;
    
    if (state)
    {
        result = (((StatePrivate*)state->_storage)->parent == 0);
    }
    
    return result;    
}

State* GetParentState(State* state)
{
    State* result = 0;
    if (state)
    {
        result = ((StatePrivate*)state->_storage)->parent;
    }
    return result;
}

int8_t StateIsChildOf(State* state, State* possibleAncestor)
{
    int8_t result = 0;
    if (state)
    {
        State* possibleParent = GetParentState(state);
        while(possibleParent)
        {
            if(possibleParent == possibleAncestor)
            {
                result = 1;
                break;
            }
            possibleParent = GetParentState(possibleParent);
        }
    }
    return result;
}
