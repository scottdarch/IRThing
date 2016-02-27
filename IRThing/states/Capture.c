/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "states/AllStates.h"

#define MAXPULSE_DURATION_MILLIS 65000
#define RESOLUTION 20
#define MAXPULSES 116
#define MINIMUM_PULSE_COUNT 2

typedef struct _CaptureDataType
{
    OnPatternCaptureFunc callback;
    OnPatternCaptureFailedFunc failureCallback;
    Pulse pulses[MAXPULSES]; // pair is high and low pulse
    uint8_t _pulseCount;
} CaptureData;

static void _notifyOfCapture(State* state)
{
    if (state && state->userData) {
        CaptureData* data = (CaptureData*)state->userData;
        if (data->callback)
        {
            data->callback(state, data->pulses, data->_pulseCount);
        }
    }
}

static void _notifyOfCaptureFailure(State* state, uint8_t failureCode)
{
    uint8_t buttonison = IS_PIN_HIGH(A, 0);

    
    if (buttonison)
    {
        SETPIN_LOW(A, 0);
    }
    
    SETPIN_LOW(A, 1); 
    _delay_ms(500);
    
    while(failureCode > 0)
    {
        SETPIN_HIGH(A, 1);
        _delay_ms(110);
        SETPIN_LOW(A, 1);
        _delay_ms(90);
        --failureCode;
    }
    
    _delay_ms(500);
    
    if (buttonison)
    {
        SETPIN_HIGH(A, 0);
    }

    if (state && state->userData) {
        CaptureData* data = (CaptureData*)state->userData;
        if (data->callback)
        {
            data->failureCallback(state);
        }
    }
}

// +--------------------------------------------------------------------------+
// | State
// +--------------------------------------------------------------------------+
StateErrorType OnEnterCaptureState(State* state, void* data, uint8_t datalen)
{
    PORTA &= ~_BV(PINA_VISUAL);
    return STATE_ERROR_NONE;
}

StateErrorType OnExitCaptureState(State* state, void* data, uint8_t datalen)
{
    PORTA &= ~_BV(PINA_VISUAL);
    return STATE_ERROR_NONE;
}

/**
* Timestable function to both pause for a fixed period and schedule some animation time
* for the main runloop.
*/
static inline uint16_t _doTickAndHandleRunloop(uint16_t lastticks)
{
    if (lastticks >= 1000)
    {
        driveMainRunLoop(1);
        lastticks += RESOLUTION - 2;
        _delay_us(RESOLUTION - 2);
        lastticks = 0;
        } else {
        lastticks += RESOLUTION;
        _delay_us(RESOLUTION);
    }
    return lastticks;
}

void OnCaptureLoop(State* state)
{
    PORTA &= ~_BV(PINA_VISUAL);
    uint8_t currentpulse = 0;
    uint16_t highpulse, lowpulse; // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length
    CaptureData* data = (CaptureData*)state->userData;
    data->_pulseCount = 0;
    uint16_t localtime = 0;
    
    disableMainLoopTimer();
    sei();
    
    while (IS_PIN_HIGH(A, 7) && StateIsEntered(state))
    {
        localtime = _doTickAndHandleRunloop(localtime);
    }
    
    while(StateIsEntered(state) && currentpulse < MAXPULSES)
    {
        highpulse = 2;
        
        PORTA |= _BV(PINA_VISUAL);
        
        while (!IS_PIN_HIGH(A, 7))
        {
            
            if (highpulse == 0xFFFF)
            {
                // Capture must complete with the ir sensor pin HIGH
                _notifyOfCaptureFailure(state, 24);
                goto STATE_CAPTURE_DONE;
            }
            else
            {
                ++highpulse;
            }
            
            localtime = _doTickAndHandleRunloop(localtime);
            if (!StateIsEntered(state))
            {
                goto STATE_CAPTURE_DONE;
            }
        }
        
        PORTA &= ~_BV(PINA_VISUAL);
        
        if (highpulse == 2)
        {
            // Expected the pin to be high.
            _notifyOfCaptureFailure(state, 4);
            goto STATE_CAPTURE_DONE;
        }
         
        data->pulses[currentpulse].high = (highpulse < 0x7F) ? 0x7F & highpulse : 0x80 | (0x7f & (highpulse >> 8));
        
        lowpulse = 1;
        
        while (IS_PIN_HIGH(A, 7))
        {
            if (lowpulse == 0xFFFF)
            {
                data->pulses[currentpulse++].low = 0xFF;
                
                if(currentpulse > MINIMUM_PULSE_COUNT)
                {
                    data->_pulseCount = currentpulse;
                    _notifyOfCapture(state);
                    currentpulse = 0;
                    goto STATE_CAPTURE_DONE;
                }
                else
                {
                    // Not enough pulses found.
                    _notifyOfCaptureFailure(state, 8);
                    goto STATE_CAPTURE_DONE;
                }
            }
            else
            {
                ++lowpulse;
            }
            
            localtime = _doTickAndHandleRunloop(localtime);
            if (!StateIsEntered(state))
            {
                goto STATE_CAPTURE_DONE;
            }
        }
        
        if (lowpulse == 1)
        {
            // Expected the pin to be low.
            _notifyOfCaptureFailure(state, 16);
            goto STATE_CAPTURE_DONE;
        }
        
        data->pulses[currentpulse++].low = (lowpulse < 0x7F) ? 0x7F & lowpulse : 0x80 | (0x7f & (lowpulse >> 8));
    }
    
    if (StateIsEntered(state))
    {
        // Too many pulses
        _notifyOfCaptureFailure(state,2);
    }
    
STATE_CAPTURE_DONE:
    cli();
    enableMainLoopTimer();
    
    return;
}

State* InitCaptureState(State* newState, State* parentState, OnPatternCaptureFunc captureCallback, OnPatternCaptureFailedFunc captureFailedCallback)
{
    newState = StateInit(newState, parentState, OnEnterCaptureState, OnExitCaptureState, OnCaptureLoop);
    if (newState) {
        CaptureData* data = malloc(sizeof(CaptureData));
        data->_pulseCount = 0;
        data->callback = captureCallback;
        data->failureCallback = captureFailedCallback;
        newState->userData = data;
    }
    return newState;
}
