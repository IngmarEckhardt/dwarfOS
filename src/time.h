#ifndef TIME_H
#define TIME_H

#ifdef __cplusplus
extern          "C" {
#endif
#include <inttypes.h>
#include <stdlib.h>


typedef uint32_t clock_t;
typedef uint32_t time_t;

#define CLOCKS_PER_SEC ((clock_t)-1)

struct tm {
    uint8_t          tm_sec;
    uint8_t          tm_min;
    uint8_t          tm_hour;
    uint8_t          tm_mday;
    uint8_t          tm_wday;
    uint8_t          tm_mon;
    uint16_t         tm_year;
    uint16_t         tm_yday;
    uint8_t          tm_isdst;
};

clock_t         clock(void);
int32_t         difftime(time_t time1, time_t time0);
time_t          mktime(struct tm * timeptr);
time_t          time(const time_t *timer);
char            *ctime (const time_t *timer);
char            *asctime(const struct tm * timeptr);
struct tm       *gmtime(const time_t * timer);
struct tm       *localtime(const time_t * timer);
size_t          strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr);

/* @} */
#ifdef __cplusplus
}
#endif

#endif              /* TIME_H  */
