/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "Indicator.h"

static void _OnIndicatorAnimate(Indicator* indicator, uint16_t timeElapsedMillis);

static inline void _changeState(Indicator* indicator, IndicatorState newState)
{
    if (newState != indicator->_state)
    {
        indicator->_state = newState;
        if (indicator->_onstateChange)
        {
            indicator->_onstateChange(indicator, newState);
        }
    }
}

static uint8_t _HandlePortMessage(RunLoopPort* port, RunLoop* runLoop, RunLoopMessageType messageType, RunLoopMessageData data)
{
    if (RUNLOOP_MESSAGE_FRAME == messageType)
    {
        _OnIndicatorAnimate((Indicator*)port->userData, data);
        return 1;
    }
    return 0;
}

void SetIndicatorMode(Indicator* indicator, IndicatorMode mode)
{
    if(indicator)
    {
        IndicatorMode oldMode = indicator->_mode;
        indicator->_mode = mode;
        indicator->_time = 0;
        indicator->_phase = 0;
        switch(mode) {
            case INDICATORMODE_BLINK_ON:
            case INDICATORMODE_BLINK_OFF:
            case INDICATORMODE_OFF:
            case INDICATORMODE_BLINK:
            case INDICATORMODE_WINK:
            _changeState(indicator, INDICATORSTATE_OFF);
            asm("nop");
            break;
        }
        switch(mode) {
            case INDICATORMODE_ON:
            _changeState(indicator, INDICATORSTATE_ON);
            asm("nop");
            break;
        }
        if (indicator->_onmodeChange)
        {
            indicator->_onmodeChange(indicator, oldMode, mode);
        }
    }
}

IndicatorMode GetIndicatorMode(Indicator* indicator)
{
    IndicatorMode result = INDICATORMODE_NONE;
    if (indicator)
    {
        result = indicator->_mode;
    }
    
    return result;
}

IndicatorState GetIndicatorState(Indicator* indicator)
{
    IndicatorState result = INDICATORSTATE_STOPPED;
    if (indicator)
    {
        result = indicator->_state;
    }
    return result;
}

void PushIndicatorMode(Indicator* indicator, IndicatorMode mode)
{
    if (indicator)
    {
        IndicatorMode currentMode = indicator->_mode;
        if (indicator->_modeStackLen == INDICATOR_MODE_STACK_SIZE) {
            uint8_t oldstackSize = INDICATOR_MODE_STACK_SIZE - 1;
            uint8_t oldstack[oldstackSize];
            memcpy(oldstack, &indicator->_modeStack[1], oldstackSize);
            memcpy(indicator->_modeStack, oldstack, oldstackSize);
            indicator->_modeStackLen = oldstackSize;
        }
        indicator->_modeStack[indicator->_modeStackLen++] = currentMode;
        SetIndicatorMode(indicator, mode);
    }
}

IndicatorMode PopIndicatorMode(Indicator* indicator)
{
    IndicatorMode result = INDICATORMODE_NONE;
    if (indicator && indicator->_modeStackLen > 0)
    {
        result = indicator->_modeStack[--indicator->_modeStackLen];
    }
    SetIndicatorMode(indicator, result);
    return result;
}

Indicator* IndicatorInit(Indicator* newIndicator, OnIndicatorModeChangeFunc onModeChange , OnIndicatorStateChangeFunc onStateChange)
{
    if (newIndicator)
    {
        newIndicator->_mode = INDICATORMODE_NONE;
        newIndicator->_state = INDICATORSTATE_STOPPED;
        newIndicator->_modeStackLen = 0;
        newIndicator->_time = 0;
        newIndicator->_phase = 0;
        newIndicator->_onmodeChange = onModeChange;
        newIndicator->_onstateChange = onStateChange;
        InitRunLoopPort(&newIndicator->_port, _HandlePortMessage);
        newIndicator->_port.userData = newIndicator;
        AddPort(&mainRunLoop, &newIndicator->_port);
        // TODO: if RUNLOOP_MAX_PORTS == AddPort then goto blink firmware error
    }
    return newIndicator;
}

static void _OnIndicatorAnimate(Indicator* indicator, uint16_t timeElapsedMillis)
{
    if (!indicator)
    {
        return;
    }
    
    indicator->_time += timeElapsedMillis;
    switch(indicator->_mode)
    {
        case INDICATORMODE_ON:
        {
            _changeState(indicator, INDICATORSTATE_ON);
            asm("nop");
            _changeState(indicator, INDICATORSTATE_STOPPED);
        }
        break;
        case INDICATORMODE_BLINK_ON:
        {
            if (0 == indicator->_phase)
            {
                if (indicator->_time >= 500)
                {
                    _changeState(indicator, INDICATORSTATE_ON);
                    ++indicator->_phase;
                    indicator->_time = 0;
                }
            }
            else if (1 == indicator->_phase)
            {
                if (indicator->_time >= 300)
                {
                    _changeState(indicator, INDICATORSTATE_OFF);
                    indicator->_time = 0;
                    ++indicator->_phase;
                }
            }
            else
            {
                if (indicator->_time >= 100)
                {
                    _changeState(indicator, INDICATORSTATE_ON);
                    asm("nop");
                    _changeState(indicator, INDICATORSTATE_STOPPED);
                }
            }
        }
        break;
        case INDICATORMODE_BLINK_OFF:
        {
            if (0 == indicator->_phase)
            {
                if (indicator->_time >= 250)
                {
                    _changeState(indicator, INDICATORSTATE_ON);
                    indicator->_time = 0;
                    ++indicator->_phase;
                }
            }
            else if (indicator->_time >= 250)
            {
                _changeState(indicator, INDICATORSTATE_OFF);
                asm("nop");
                _changeState(indicator, INDICATORSTATE_STOPPED);
            }
        }
        break;
        case INDICATORMODE_BLINK:
        {
            if (0 == indicator->_phase)
            {
                if (indicator->_time >= 100)
                {
                    _changeState(indicator, INDICATORSTATE_ON);
                    indicator->_time = 0;
                    ++indicator->_phase;
                }
            }
            else if (indicator->_time >= 100)
            {
                _changeState(indicator, INDICATORSTATE_OFF);
                indicator->_time = 0;
                indicator->_phase = 0;
            }
        }
        break;
        case INDICATORMODE_WINK:
        {
            switch (indicator->_phase)
            {
                case 0:
                {
                    // On long
                    if (indicator->_time >= 75)
                    {
                        _changeState(indicator, INDICATORSTATE_ON);
                        indicator->_time = 0;
                        indicator->_phase++;
                    }
                }
                break;
                case 1:
                {
                    // wink off
                    if (indicator->_time >= 1500)
                    {
                        _changeState(indicator, INDICATORSTATE_OFF);
                        indicator->_time = 0;
                        indicator->_phase++;
                    }
                }
                break;
                case 2:
                {
                    // wink on
                    if (indicator->_time >= 75)
                    {
                        _changeState(indicator, INDICATORSTATE_ON);
                        indicator->_time = 0;
                        indicator->_phase++;
                    }
                }
                break;
                case 3:
                {
                    // wink off
                    if (indicator->_time >= 100)
                    {
                        _changeState(indicator, INDICATORSTATE_OFF);
                        indicator->_time = 0;
                        indicator->_phase = 0;
                    }
                }
                break;
            }
        }
        break;
        case INDICATORMODE_OFF:
        {
            _changeState(indicator, INDICATORSTATE_OFF);
            asm("nop");
            _changeState(indicator, INDICATORSTATE_STOPPED);
        }
        break;
        case INDICATORMODE_NONE:
        {
            _changeState(indicator, INDICATORSTATE_STOPPED);
        }
        break;
    }
}
