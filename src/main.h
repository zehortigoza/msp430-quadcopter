#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <legacymsp430.h>
#include <msp430.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "motors.h"
#include "radio.h"
#include "protocol.h"
#include "i2c.h"
#include "mpu6050.h"

void agent_init(void);

#endif
