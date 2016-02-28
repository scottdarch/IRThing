/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/


#ifndef INDICATOR_H_
#define INDICATOR_H_

#include "Framework.h"
#include "tinker/RunLoop.h"

#define INDICATORMODE_NONE 0
#define INDICATORMODE_ON 1
#define INDICATORMODE_BLINK_ON 2
#define INDICATORMODE_BLINK_OFF 3
#define INDICATORMODE_BLINK 4
#define INDICATORMODE_WINK 5
#define INDICATORMODE_OFF 0xFF

#define INDICATORSTATE_STOPPED 0
#define INDICATORSTATE_OFF 1
#define INDICATORSTATE_ON 2

#define INDICATOR_MODE_STACK_SIZE 2

struct _IndicatorType;

typedef uint8_t IndicatorMode;
typedef uint8_t IndicatorState;

typedef void (*OnIndicatorModeChangeFunc)(struct _IndicatorType*, IndicatorMode oldMode, IndicatorMode newMode);
typedef void (*OnIndicatorStateChangeFunc)(struct _IndicatorType*, IndicatorState state);

typedef struct _IndicatorType
{
    IndicatorMode _mode;
    IndicatorMode _modeStack[INDICATOR_MODE_STACK_SIZE];
    uint8_t _modeStackLen;
    IndicatorState _state;
    uint16_t _time;
    uint8_t _phase;
    OnIndicatorModeChangeFunc _onmodeChange;
    OnIndicatorStateChangeFunc _onstateChange;
    RunLoopPort _port;
} Indicator;

Indicator* IndicatorInit(Indicator* newIndicator, OnIndicatorModeChangeFunc onModeChange, OnIndicatorStateChangeFunc onStateChange);
void SetIndicatorMode(Indicator* indicator, IndicatorMode mode);
void PushIndicatorMode(Indicator* indicator, IndicatorMode mode);
IndicatorMode PopIndicatorMode(Indicator* indicator);

IndicatorMode GetIndicatorMode(Indicator* indicator);
IndicatorState GetIndicatorState(Indicator* indicator);

#endif /* INDICATOR_H_ */