# ClariMIDI
## Contents
* [Overview](#overview)
* 3D Printing
* Pico Circuitry
* Assembly and Wiring
* Operation Guide

## Overview {#overview}
ClariMIDI is a MIDI wind controller based closely on a standard (Boehm) clarinet layout.

* 3D printed body
* 7 touch keys (the 'holes')
* 18 press keys (the 'keys')
* Breath sensor
* USB MIDI
* OLED menu
* Raspberry Pico microcontroller

It is specifically targetted at clarinettists who want to play a wind controller.
Standard clarinet fingerings are used, and the key layout mimics a normal clarinet.

N.B. This is a MIDI controller, it needs to be connected via USB-MIDI to a synthesizer to make
any sound.

### Parts List
#### 3D Printed
You will need to print the following parts...
* Mouthpiece
* BreathSensor (the body)
* SensorRack (adjustable rack for the breath sensor)
* SensorRackScrew 
* Body_V2 (the front of the body)
* Body_V2-BackBody (upper back of the body)
* ??? (lower back of the body)
* 5 x SwitchBoxes-SingleBody
* 3 x SwitchBoxes-DoubleBody
* 1 x SwitchBoxes-TripleBody
* 1 x SwitchBoxes-QuadBody
* 18 x KeyStem
* ??? (key tops)

The FreeCAD designs and .3mf models are in the `3D` directory.

#### Electronics
* Raspberry Pico H
* Soldered prototyping board (see photos)
* 330 Ohm resistor
* 1k Ohm resistor
* 18 x silent keyswitches
* 2.4cm (0.96") SSD1306 128x64 OLED display with I2C interface
* CNY70 reflective optical sensor
* MPR121 capacitative touch sensors I2C module (Adafruit type 1982 or equivalent)

#### Hardware
* White balloon (the softer the better)
* Hook-up wire and various Dupont cables
* 7 x 'brass' drawing pins ("pushpins")
* Superglue or equivalent
* Hot glue gun