/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
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

typedef uint8_t (*OnHandlePortMessageFunc)(struct _RunLoopPortType*, struct _RunLoopType*, RunLoopMessageType, RunLoopMessageData);

typedef struct _RunLoopPortType
{
    OnHandlePortMessageFunc handlePortMessage;
    void* userData;
} RunLoopPort;

RunLoopPort* InitRunLoopPort(RunLoopPort* newPort, OnHandlePortMessageFunc handler);

typedef uint8_t (*RunModeFunc)(struct _RunLoopType*, RunLoopMessageType, RunLoopMessageData);

typedef struct _RunLoopType
{
    RunLoopPort* ports[RUNLOOP_MAX_PORTS];
    RunModeFunc runMode;
} RunLoop;

RunLoop* InitRunLoop(RunLoop* newLoop);

/**
 * Set a port for a runloop.
 * @param  runLoop      The runloop to set the port on.
 * @param  portNumber   The port number between 0 and RUNLOOP_MAX_PORTS - 1.
 * @param  port         The port object.
 * @return The port that was replaced by this port or 0 if no port had been
 *         set for the given port number.
 */
RunLoopPort* SetPort(RunLoop* runLoop, uint8_t portNumber, RunLoopPort* port);

RunLoopPort* RemovePort(RunLoop* runLoop, uint8_t portNumber);

#endif /* RUNLOOP_H_ */
