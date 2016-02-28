/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/
/**
 * \file Framework.h
 * Common defines, includes, types, and method signatures for 
 * the IR Thing firmware.
 */


#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_


// ATTiny Running at 20MHz (external crystal)
#define F_CPU 20000000UL
// MCU powered by 5v
#define VCC 5

#include <avr/power.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

// +--------------------------------------------------------------------------+
// | HELPER MACROS
// +--------------------------------------------------------------------------+
#define IS_PIN_HIGH(xPORT, NUM) ((PIN ##xPORT & (1<<PIN ##xPORT##NUM)) ? 0x01 : 0x00)
#define SETPIN_HIGH(xPORT, NUM) PORT ##xPORT |= (1<<PIN ##xPORT##NUM)
#define SETPIN_LOW(xPORT, NUM) PORT ##xPORT &= ~(1<<PIN ##xPORT##NUM)
#define TOGGLEPIN(xPORT, NUM) PORT ##xPORT ^= (1<<BPIN ##xPORT##NUM)
#define ENABLE_EXTERNAL_INTERRUPT(EXTINTNUM) GIMSK |= (1<<INT##EXTINTNUM);
#define DISABLE_EXTERNAL_INTERRUPT(EXTINTNUM) GIMSK &= ~(1<<INT##EXTINTNUM);

// +--------------------------------------------------------------------------+
// | PINS
// +--------------------------------------------------------------------------+
#define PINA_RUNNING  PINA0
#define PINA_VISUAL   PINA1
#define PINA_IR_IN    PINA7
#define PINB_RUNBUTT  PINB2
#define PINA_IR_OUT   PINA2
#define PINA_PERIPH   PINA3

// +--------------------------------------------------------------------------+
// | RUN LOOPS
// +--------------------------------------------------------------------------+
#include "tinker/RunLoop.h"

/**
 *
 */
extern RunLoop mainRunLoop;

#define RUNLOOP_MESSAGE_FRAME 0

#define RUNLOOP_PORT_INDICATOR 0
#define RUNLOOP_PORT_BUTTON 1

extern void disableMainLoopTimer();
extern void enableMainLoopTimer();
extern void driveMainRunLoop(uint8_t timeSinceLastRunMillis);

#endif /* FRAMEWORK_H_ */
