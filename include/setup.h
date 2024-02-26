#ifndef DWARFOS_SETUP_H
#define DWARFOS_SETUP_H


#include "mcu_clock.h"
#include "string_repository.h"

#ifndef INIT_TIME
//2024-02-18 19:50:00(CET)
#define INIT_TIME ((uint32_t) 761601000)
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 19200UL
#endif

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)


void setupMcu(McuClock * mcuClock, StringRepository * stringRepository);

#endif //DWARFOS_SETUP_H
