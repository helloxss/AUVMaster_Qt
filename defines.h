#ifndef DEFINES_H
#define DEFINES_H

#include "QDebug"

#define QSL(x) QString::fromLocal8Bit(x)
#define SET_BIT_AS_1(var,bit) (var |= (1<<bit))
#define SET_BIT_AS_0(var,bit) (var &= ~(1<<bit))

#define PROPELLER_PORT "COM6"
#define SWITCH_ARDUINO_PORT "COM3"
#define LED_ARDUINO_PORT "COM5"
#define DEPTH_SENSOR_PORT "COM1"
#define POSTURE_SENSOR_PORT "COM2"

#endif // DEFINES_H
