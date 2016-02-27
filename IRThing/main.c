/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "Framework.h"
#include "Button.h"
#include "State.h"
#include "states/AllStates.h"
#include "Machine.h"
#include "Indicator.h"


// +--------------------------------------------------------------------------+
// | MAIN RUN LOOP
// +--------------------------------------------------------------------------+

void ensureMainRunLoopTimer();

// +--------------------------------------------------------------------------+
// | STATES
// +--------------------------------------------------------------------------+
typedef struct _DeviceStateT
{
    int isInterrupted : 1;
    int wasLongPress : 1;
    int reserved : 6;
} DeviceState;

static DeviceState PRDS;

State RootState;
State RunningState;
State VisualizeState;
State CapturingState;
State RepeatingState;

// Optimization. Same as GetMachineFocus(&masterMachine);
State* focusedState;

Machine masterMachine;

void Shutdown()
{
    SetMachineState(&masterMachine, &RootState);
}

// +--------------------------------------------------------------------------+
// | BUTTON
// +--------------------------------------------------------------------------+
static Button powerButton;
static Indicator powerButtonIndicator;

void onIndicatorStateChange(Indicator* indicator, IndicatorState state)
{
    switch(state)
    {
        case INDICATORSTATE_OFF:
        {
            SETPIN_LOW(A, 0);
            ensureMainRunLoopTimer();
        }
        break;
        case INDICATORSTATE_ON:
        {
            SETPIN_HIGH(A, 0);
            ensureMainRunLoopTimer();
        }
        break;
    }
}

void OnButtonEvent(Button* target, ButtonEventType event)
{
    switch(event) {
        case BUTTON_EVENT_UP:
        {
            PopIndicatorMode(&powerButtonIndicator);
            if (PRDS.wasLongPress && !focusedState)
            {
                SetIndicatorMode(&powerButtonIndicator, INDICATORMODE_BLINK_OFF);
            }
            else if (!focusedState)
            {
                SetMachineState(&masterMachine, &VisualizeState);
            }
            else if (focusedState == &VisualizeState)
            {
                SetMachineState(&masterMachine, &CapturingState);
            }
            else
            {
                StateHandleInterrupt(focusedState, STATE_INT_BUTTON_CLICK);
            }
            PRDS.isInterrupted = 0;
            ENABLE_EXTERNAL_INTERRUPT(0);
        }
        break;
        case BUTTON_EVENT_DOWN:
        {
            PushIndicatorMode(&powerButtonIndicator, INDICATORMODE_ON);
            PRDS.wasLongPress = 0;
        }
        break;
        case BUTTON_EVENT_LONG_PRESS:
        {
            PRDS.wasLongPress = 1;
            Shutdown();
        }
        break;
    }
}

ISR(INT0_vect)
{
    DISABLE_EXTERNAL_INTERRUPT(0);
    PRDS.isInterrupted = 1;
    ensureMainRunLoopTimer();
}


// +--------------------------------------------------------------------------+
// | STATE HANDLERS
// +--------------------------------------------------------------------------+

void OnCapturePattern(State* captureState, Pulse* pulses, uint8_t pulseCount)
{
    SetMachineStateWData(&masterMachine, &RepeatingState, pulses, pulseCount);
}

void OnCapturePatternFailed(State* captureState)
{
    //SetMachineState(&masterMachine, &VisualizeState);
}

void OnStateChange(Machine* machine, State* oldState, State* newState)
{
    focusedState = IsRootState(newState) ? 0 : newState;
    
    if (!oldState || IsRootState(oldState))
    {
        SetIndicatorMode(&powerButtonIndicator, INDICATORMODE_BLINK_ON);
    }
    else if (newState == &CapturingState)
    {
        SetIndicatorMode(&powerButtonIndicator, INDICATORMODE_BLINK);
    }
    else if (newState == &VisualizeState)
    {
        SetIndicatorMode(&powerButtonIndicator, INDICATORMODE_ON);
    }
    else if (newState == &RepeatingState)
    {
        SetIndicatorMode(&powerButtonIndicator, INDICATORMODE_WINK);
    }
}


// +--------------------------------------------------------------------------+
// | RUN LOOP
// +--------------------------------------------------------------------------+
#define _MAIN_RUNLOOP_TIMER_ENABLED 0x1
#define _MAIN_RUNLOOP_TIMER_ACTIVE 0x2

static const uint8_t timerPeriodMillis = 1;

RunLoop mainRunLoop;

static uint16_t elapsedLoopDriveTimeMillis;
static uint8_t runloopTimerState;

void ensureMainRunLoopTimer()
{
    // Try to drive the runloop from timer1. If interrupts are disabled then someone
    // will need to call driveMainRunLoop manually.
    // 20MHz / 64 = 312kHz / 255 = ~123 overflows per second or ~.81msec per overflow
    runloopTimerState |= _MAIN_RUNLOOP_TIMER_ACTIVE;
    if (runloopTimerState & _MAIN_RUNLOOP_TIMER_ENABLED)
    {
        TCCR0B = _BV(CS01) | _BV(CS00);
    }
}

void driveMainRunLoop(uint8_t timeSinceLastRunMillis)
{
    if (PRDS.isInterrupted)
    {
        mainRunLoop.runMode(&mainRunLoop, RUNLOOP_MESSAGE_BUTTONTEST, IS_PIN_HIGH(B, 2));
    }
    else if (INDICATORSTATE_STOPPED == GetIndicatorState(&powerButtonIndicator))
    {
        elapsedLoopDriveTimeMillis = 0;
        runloopTimerState &= ~_MAIN_RUNLOOP_TIMER_ACTIVE;
        TCCR0B = 0;
    }
    else if (elapsedLoopDriveTimeMillis >= 16)
    {
        mainRunLoop.runMode(&mainRunLoop, RUNLOOP_MESSAGE_FRAME, (RunLoopMessageData)elapsedLoopDriveTimeMillis);
        elapsedLoopDriveTimeMillis = 0;
    }
    else
    {
        elapsedLoopDriveTimeMillis += timeSinceLastRunMillis;
    }
    
}

void disableMainLoopTimer()
{
    runloopTimerState &= ~_MAIN_RUNLOOP_TIMER_ENABLED;
    TCCR0B = 0;
}

void enableMainLoopTimer()
{
    runloopTimerState |= _MAIN_RUNLOOP_TIMER_ENABLED;
    ensureMainRunLoopTimer();
}


ISR(TIM0_OVF_vect)
{
    driveMainRunLoop(timerPeriodMillis);
}


// +--------------------------------------------------------------------------+
// | MAIN!
// +--------------------------------------------------------------------------+
static inline void init()
{
    cli();
    focusedState = 0;
    memset(&PRDS, 0, sizeof(PRDS));
    elapsedLoopDriveTimeMillis = 0;
    runloopTimerState = 0x3;
    
    InitRunLoop(&mainRunLoop);
    IndicatorInit(&powerButtonIndicator, 0, onIndicatorStateChange);
    ButtonInit(&powerButton, OnButtonEvent);
    
    InitRepeatState(&RepeatingState, &RunningState);
    InitCaptureState(&CapturingState, &RunningState, OnCapturePattern, OnCapturePatternFailed);
    InitVisualizeState(&VisualizeState, &RunningState);
    InitRunningState(&RunningState, &RootState, (State*[]){&VisualizeState, &CapturingState, &RepeatingState}, 3);
    StateInitWSubstates(&RootState, 0, 0, 0, 0, (State*[]){&RunningState}, 0);
    
    MachineInit(&masterMachine, OnStateChange);

    // +---[POWER SETTINGS]---------------------------------------------------+
    ACSR = 0;                               /**< Disable analog comparator. */
    /* Enable pullup resistors, enable INT0 when pulled low. */
    MCUCR &= ~((1<<PUD) | (1<<ISC01) | (1<<ISC00));    /**< INT0 low level */
    PRR = _BV(PRADC) | _BV(PRTIM1);
    
    // +---[OTHER SETUP]------------------------------------------------------+
    PORTA = _BV(PINA_RUNNING) | _BV(PINA_VISUAL) | _BV(PINA_IR_IN);
    PORTB = _BV(PINB_RUNBUTT);
    DDRA = _BV(PINA_RUNNING) | _BV(PINA_VISUAL) | _BV(PINA_IR_OUT) | _BV(PINA_PERIPH) | _BV(PINA4);
    ENABLE_EXTERNAL_INTERRUPT(0);
    
    TCCR0A = 0;
    TCCR0B = 0;
    TIMSK0 |= _BV(TOIE0);
    
    // +----------------------------------------------------------------------+
    //  Blink all the indicators for half a second and let peripherals settle.
    _delay_ms(500);
    
    PORTA &= ~(_BV(PINA_VISUAL) | _BV(PINA_RUNNING));
    sei();
}

int main(void)
{
    init();

    while(1)
    {
        cli();
        if (!focusedState)
        {
            // No running states. Wait for all timer based activity to cease then go to sleep.
            // INT0 can wake us back up.
            if (!PRDS.isInterrupted && INDICATORSTATE_STOPPED == GetIndicatorState(&powerButtonIndicator))
            {
                sei();
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                sleep_enable();
                sleep_bod_disable();
                sleep_cpu();
                sleep_disable();
                cli();
            }
        }
        else
        {
            focusedState->OnLoop(focusedState);
        }
        sei();
    }
    return 0;
}
