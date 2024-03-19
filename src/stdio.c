#include <dwarf-os/stdio.h>
#include <avr/pgmspace.h>
#include <stdio.h>

int16_t puts_PF(uint32_t farPointerToString) {
#ifdef __AVR_HAVE_ELPM__
    char charToPut = 0;
    while ((charToPut = (char) pgm_read_byte_far(farPointerToString))) {
        if ((stdout->flags & __SWR) != 0) {
            if ((stdout->put(charToPut, stdout) != 0)) {
                return EOF;
            } else {
                farPointerToString++;
            }
        }
    }
#endif
    return 0;
}