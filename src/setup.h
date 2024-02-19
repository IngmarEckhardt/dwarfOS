#ifndef MCU_SETUP_H
#define MCU_SETUP_H

#include <avr/io.h>
#include "mcu_clock.h"

void setupMcu();
void setCPUParamRegister();

//2024-02-18 19:50:00(CET)
#define INIT_TIME ((uint32_t) 761601000)
#define F_CPU 16000000UL
#define BAUD 57600
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)

#endif // MCU_SETUP_H
