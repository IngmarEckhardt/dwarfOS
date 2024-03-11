#ifndef DWARFOS_PGM_TEXTFILE_GENERATOR_H
#define DWARFOS_PGM_TEXTFILE_GENERATOR_H

#include <stdint.h>

void convertTextArrayToProgMemTextFiles(const char * textsArray[], uint16_t amount, const uint16_t bordersArray[],
                                        uint8_t amountBorders, char * prefix);

#endif //DWARFOS_PGM_TEXTFILE_GENERATOR_H