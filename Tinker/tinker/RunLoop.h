/*
Copyright 2016 Scott A Dixon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#ifndef RUNLOOP_H_
#define RUNLOOP_H_

#ifndef RUNLOOP_MAX_PORTS
#define RUNLOOP_MAX_PORTS 2
#endif

#include <stdint.h>

struct _RunLoopType;
struct _RunLoopPortType;

typedef uint8_t RunLoopMessageType;
typedef uint16_t RunLoopMessageData;

/**
 * Port handler function type. Invoked as part of
 * a runLoop mode. See \link RunLoop::runMode \endlink
 * for full documentation.
 * \param  port     The port the handler message is being sent to.
 * \param  runloop  The runloop that is dispatching this message.
 * \param  message  A message identifier as provided to the RunLoop mode.
 * \param  data     Opaque data provided to the RunLoop mode.
 */
typedef uint8_t (*OnHandlePortMessageFunc)(struct _RunLoopPortType* port, struct _RunLoopType* runloop, RunLoopMessageType message, RunLoopMessageData data);

/**
 * \struct RunLoopPort
 * A RunLoopPort type.
 */
typedef struct _RunLoopPortType
{
    /**
     * Set by the InitRunLoopPort initializer.
     */
    OnHandlePortMessageFunc handlePortMessage;

    /**
     * Opaque pointer available for external use.
     * This pointer is neither read nor written by the
     * RunLoopPort and RunLoop objects.
     */
    void* userData;
} RunLoopPort;

/**
 * Objective-C style object initializer for a RunLoopPort object.
 * \param  newPort  The port object to initialize.
 * \param  handler  A handler function to set on the port object.
 * \return A pointer to the initialized port object.
 */
RunLoopPort* InitRunLoopPort(RunLoopPort* newPort, OnHandlePortMessageFunc handler);

/**
 * RunLoop "run mode" function. See \link RunLoop::runMode \endlink for example usage.
 * \param  runLoop      The runloop to send the message to.
 * \param  messageType  The message type. This identifier is opaque to the Tinker framework.
 * \param  messageData  Opaque data to pass to \link RunLoopPort \endlink objects.
 */
typedef uint8_t (*RunModeFunc)(struct _RunLoopType* runLoop, RunLoopMessageType messageType, RunLoopMessageData messageData);

/**
 * \struct RunLoop
 * Object type for Cocoa style RunLoop. This (obviously) is a vastly
 * simplified version of that iOS class.
 */
typedef struct _RunLoopType
{
    RunLoopPort* _ports[RUNLOOP_MAX_PORTS];
    uint8_t _portCount;

    /**
     * The method to invoke when driving this runloop. For example to drive
     * a runloop from a timer interrupt (AVR example given here) do:
     * <pre>
     * ISR(TIM0_OVF_vect)
     * {
     *     myTimer0RunLoop->runMode(myTimer0RunLoop, SOME_MESSAGE_ID, 0);
     * }
     * </pre>
     */
    RunModeFunc runMode;
} RunLoop;

/**
 * Objective C style object initializer for the runloop.
 * \param  newLoop  The runloop to initialize.
 * \return The initialized runloop.
 */
RunLoop* InitRunLoop(RunLoop* newLoop);

/**
 * Set a port for a runloop.
 * \param  runLoop      The runloop to set the port on.
 * \param  portNumber   The port number between 0 and RUNLOOP_MAX_PORTS - 1.
 * \param  port         The port object.
 * \return The port that was replaced by this port or 0 if no port had been
 *         set for the given port number.
 */
RunLoopPort* SetPort(RunLoop* runLoop, uint8_t portNumber, RunLoopPort* port);

/**
 * Add a port to a runloop.
 * \param  runLoop      The runloop to add the port to.
 * \param  port         The port object to add.
 * \return The port number assigned to the added port or
 *         RUNLOOP_MAX_PORTS if the port could not be added.
 */
uint8_t AddPort(RunLoop* runLoop, RunLoopPort* port);

/**
 * Remove a port from a runloop.
 * \param  runLoop  The runloop to remove the port from.
 * \param  portNumber The port number to remove.
 * \return The removed port or 0 if the specified port number
 *         was not set for this runloop or was out of range.
 */
RunLoopPort* RemovePort(RunLoop* runLoop, uint8_t portNumber);

#endif /* RUNLOOP_H_ */
