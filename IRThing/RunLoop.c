/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "RunLoop.h"
#include <string.h>

RunLoopPort* InitRunLoopPort(RunLoopPort* newPort, OnHandlePortMessageFunc handler)
{
    if (newPort)
    {
        newPort->handlePortMessage = handler;
    }
    return newPort;
}

uint8_t _RunMode(RunLoop* runLoop, RunLoopMessageType message, RunLoopMessageData data)
{
    uint8_t handled = 0;
    if (runLoop)
    {
        RunLoopPort* port;
        for(uint8_t i = 0; i < RUNLOOP_MAX_PORTS && !handled; ++i)
        {
            port = runLoop->ports[i];
            if (port)
            {
                handled = port->handlePortMessage(port, runLoop, message, data);
            }
        }
    }
    return handled;
} 

RunLoop* InitRunLoop(RunLoop* newLoop)
{
    if (newLoop)
    {
        memset(newLoop->ports, 0, sizeof(RunLoopPort*) * RUNLOOP_MAX_PORTS);
        newLoop->runMode = _RunMode;
    }
    return newLoop; 
}

RunLoopPort* SetPort(RunLoop* runLoop, uint8_t portNumber, RunLoopPort* port)
{
    RunLoopPort* displaced = 0;
    if (runLoop)
    {
        if (portNumber < RUNLOOP_MAX_PORTS && port && port->handlePortMessage)
        {
            displaced = runLoop->ports[portNumber];
            runLoop->ports[portNumber] = port;
        }
    }
    return displaced;
}

RunLoopPort* RemovePort(RunLoop* runLoop, uint8_t portNumber)
{
    RunLoopPort* removed = 0;
    if (runLoop)
    {
        if (portNumber < RUNLOOP_MAX_PORTS)
        {
            removed = runLoop->ports[portNumber];
            runLoop->ports[portNumber] = 0;
        }
    }
    return removed;
}
