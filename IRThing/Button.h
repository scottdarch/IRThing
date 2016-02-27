/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#ifndef BUTTON_H_
#define BUTTON_H_

#include "Framework.h"

#ifndef BUTTON_DOWNSAMPLES_IS_LONGPRESS
#define BUTTON_DOWNSAMPLES_IS_LONGPRESS 1500
#endif

#define BUTTON_EVENT_DOWN       1
#define BUTTON_EVENT_UP         2
#define BUTTON_EVENT_LONG_PRESS 3

typedef uint8_t ButtonEventType;

struct _ButtonType;

typedef void (*OnButtonEventFunc)(struct _ButtonType* target, ButtonEventType event);

typedef struct _ButtonType
{
    uint16_t _state;
    uint16_t _downsamples;
    OnButtonEventFunc _onButtonEvent;
    RunLoopPort _port;
} Button;

void ButtonInit(Button* button, OnButtonEventFunc handler);

#endif /* BUTTON_H_ */
