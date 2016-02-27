```
/*
~          +-+
~ IR THING |  ) ... ... ..     ... ... ..     ... ... ..     ... ... ..
~          +-+
*/
```


Wherein we present a silly little project. A tool for working with a fast fading
technology: Infrared remote control protocols.

This repository contains a firmware for the first prototype board built around an [Atmel-ATtiny24](/docs/Atmel-ATtiny24-ATtiny44-ATtiny84_Datasheet-Complete.pdf).
It uses a basic hierarchical state machine to coordinate control of GPIO between
three modes: visualize, capture, repeat.

![Dev board prototype 0](/docs/devboard.jpg)

## Main Board Pinout

![Main Board](/docs/pinout_level0.jpg)

## Riser Board Pinout

![Riser Board](/docs/pinout_level1.jpg)
