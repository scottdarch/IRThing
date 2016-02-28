/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "tinker/Button.h"
#include "tinker/RunLoop.h"

#define BUTTON_STATE_UNSTABLE 0
#define BUTTON_STATE_CLOSED   4
#define BUTTON_STATE_OPEN     5

#define BUTTON_STATE_QUEUE_LEN 13U
#define BUTTON_STATE_DOWN_BIT 0x8000U
#define BUTTON_READING_BITS_MASK 0x1FFFU

#define IS_BUTTON_PIN_HIGH(BUTTON) ((BUTTON->_port & (1<<BUTTON->_pin)) ? 0x01 : 0x00)

uint8_t _testButtonPin(Button* self, uint8_t isPinHigh)
{
    register volatile uint16_t state = self->_state;
    // save the top 3 bits...
    register volatile uint8_t downState = (state >> 8) & ~(BUTTON_READING_BITS_MASK >> 8);

    register volatile uint8_t returnValue;

    // shift a new pin reading into the queue and restore the top 3 bits.
    state = ((state << 1) & BUTTON_READING_BITS_MASK) | (downState << 8) | !isPinHigh;
    
    // UP
    // 1xx1 0000 0000 0000
    // DOWN
    // 0xx0 1111 1111 1111
    // OPEN
    // xxx0 0000 0000 0000
    // CLOSED
    // xxx1 1111 1111 1111
    if (state == (BUTTON_STATE_DOWN_BIT | (1 << (BUTTON_STATE_QUEUE_LEN-1))))
    {
        // Mark the pin as "up".
        state &= ~BUTTON_STATE_DOWN_BIT;
        returnValue = BUTTON_EVENT_UP;
    }
    else if (state == (BUTTON_READING_BITS_MASK >> 1))
    {
        // mark the pin as being "down". "up" can only occur
        // if the pin was already down.
        state |= BUTTON_STATE_DOWN_BIT;
        self->_downsamples = 1;
        returnValue = BUTTON_EVENT_DOWN;
    }
    else if ((state & BUTTON_READING_BITS_MASK) == 0x00)
    {
        returnValue = BUTTON_STATE_OPEN;
    }
    else if ((state & BUTTON_READING_BITS_MASK) == BUTTON_READING_BITS_MASK)
    {
        if (BUTTON_DOWNSAMPLES_IS_LONGPRESS == self->_downsamples)
        {
            self->_downsamples = BUTTON_DOWNSAMPLES_IS_LONGPRESS + 1;
            returnValue = BUTTON_EVENT_LONG_PRESS;
        }
        else
        {
            // Don't let this wrap around and trigger another longpress.
            if (self->_downsamples < BUTTON_DOWNSAMPLES_IS_LONGPRESS)
            {
                ++(self->_downsamples);
            }
            returnValue = BUTTON_STATE_CLOSED;
        }
    }
    else
    {
        returnValue = BUTTON_STATE_UNSTABLE;
    }

    self->_state = state;
    return returnValue;
}

uint8_t _HandlePortMessage(RunLoopPort* port, RunLoop* runLoop, RunLoopMessageType messageType, RunLoopMessageData data)
{
    if (RUNLOOP_MESSAGE_BUTTONTEST == messageType)
    {
        Button* self = (Button*)port->userData;
        uint8_t isPinHigh = (0xFF & data);
        const uint8_t state = _testButtonPin(self, isPinHigh);
        switch(state) {
            case BUTTON_EVENT_UP:
            {
                self->_onButtonEvent(self, BUTTON_EVENT_UP);
            }
            break;
            case BUTTON_EVENT_DOWN:
            {
                self->_onButtonEvent(self, BUTTON_EVENT_DOWN);
            }
            break;
            case BUTTON_EVENT_LONG_PRESS:
            {
                self->_onButtonEvent(self, BUTTON_EVENT_LONG_PRESS);
            }
            break;
        }
        return 1;
    }
    return 0;
}

Button* ButtonInit(Button* button, OnButtonEventFunc handler, RunLoop* runLoop, uint8_t buttonPort)
{
    if (button) {
        button->_state = 0;
        button->_downsamples = 0;
        button->_onButtonEvent = handler;
        InitRunLoopPort(&button->_port, _HandlePortMessage);
        button->_port.userData = button;
        SetPort(runLoop, buttonPort, &button->_port);
    }
    return button;        
}
