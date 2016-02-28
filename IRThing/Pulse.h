/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/

#ifndef PULSE_H_
#define PULSE_H_

/**
 * \struct Pulse
 * Type that contains timings for high and low pulse emitted by an IR remote.
 */
typedef struct _PulseType
{
    /**
     * High pulse. Interpretation will vary by encoding. Read docs in the
     * method populating this data to understand how to interpret it.
     */
    uint8_t high;
    
    /**
     * Low pulse. Interpretation will vary by encoding. Read docs in the
     * method populating this data to understand how to interpret it.
     */
    uint8_t low;

} Pulse;


#endif /* PULSE_H_ */