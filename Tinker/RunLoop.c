/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#include "tinker/RunLoop.h"
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
        for(uint8_t i = 0; i < runLoop->_portCount && !handled; ++i)
        {
            port = runLoop->_ports[i];
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
        memset(newLoop->_ports, 0, sizeof(RunLoopPort*) * RUNLOOP_MAX_PORTS);
        newLoop->_portCount = 0;
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
            displaced = runLoop->_ports[portNumber];
            runLoop->_ports[portNumber] = port;
        }
    }
    return displaced;
}

uint8_t AddPort(RunLoop* runLoop, RunLoopPort* port)
{
    uint8_t portNumber = RUNLOOP_MAX_PORTS;
    if (runLoop && runLoop->_portCount < RUNLOOP_MAX_PORTS)
    {
        portNumber = runLoop->_portCount++;
        runLoop->_ports[portNumber] = port;
    }
    return portNumber;
}

RunLoopPort* RemovePort(RunLoop* runLoop, uint8_t portNumber)
{
    RunLoopPort* removed = 0;
    if (runLoop)
    {
        if (portNumber < RUNLOOP_MAX_PORTS)
        {
            removed = runLoop->_ports[portNumber];
            runLoop->_ports[portNumber] = 0;
        }
    }
    return removed;
}
