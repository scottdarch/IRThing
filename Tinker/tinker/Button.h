/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#ifndef BUTTON_H_
#define BUTTON_H_

#include "tinker/RunLoop.h"
#include <stdint.h>

/**
 * Type able to store any BUTTON_EVENT_XXXX value.
 */
typedef uint8_t ButtonEventType;

/**
 * Debounced button down event.
 */
#define BUTTON_EVENT_DOWN       1

/**
 * Debounced button up event.
 */
#define BUTTON_EVENT_UP         2

/**
 * Debounced button long-press event.
 */
#define BUTTON_EVENT_LONG_PRESS 3

struct _ButtonType;

/**
 * Function type for button event handler.
 * \param source    The button the event originated from.
 * \param event     The event type.
 */
typedef void (*OnButtonEventFunc)(struct _ButtonType* source, ButtonEventType event);

/**
 * \struct Button
 * 
 * Button object. 
 */
typedef struct _ButtonType
{
    uint16_t _state;
    uint16_t _downsamples;
    OnButtonEventFunc _onButtonEvent;
    RunLoopPort _port;
} Button;

/**
 * Objective-C style button object initializer.
 * \param button     The button instance to initialize.
 * \param handler    A handler function to invoke when events occur for the
 *                   button instance.
 * \param runLoop    The runloop used to process GPIO state and generate button events.
 * \return A pointer to the initialized button instance.
 */
Button* ButtonInit(Button* button, OnButtonEventFunc handler, RunLoop* runLoop);

// +--------------------------------------------------------------------------+
// | BUTTON TUNING
// +--------------------------------------------------------------------------+
#ifndef BUTTON_DOWNSAMPLES_IS_LONGPRESS
#define BUTTON_DOWNSAMPLES_IS_LONGPRESS 1500
#endif

// TODO: Wart. Find a dynamic way to assign runloop message ids.
#define RUNLOOP_MESSAGE_BUTTONTEST 1

#endif /* BUTTON_H_ */
