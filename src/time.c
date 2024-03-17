#include "time.h"
#include "ascii_helper.h"

uint32_t (* takeTimeFromClock)(void) = NULL;

//helper functions
uint16_t calcYear(uint32_t * days);

uint8_t daysInMonth(uint16_t year, uint8_t month);

uint8_t calcUtcOffset(uint32_t epochTimeY2K);

uint8_t calcMonth(uint32_t * days, uint16_t year);

uint8_t isDst(uint16_t year, uint8_t month, uint8_t day);

uint8_t calcZellerCongruence(uint16_t year, uint8_t month, uint8_t day);

uint8_t isLeapYear(uint16_t year);

void formatString(char * resultString, struct tm * timeStructPtr);

void addTimezone(char * result, uint8_t timezoneFlag);

//expecting no mcu library use this function, because it is per default unknown in a mcu environment without rtc
//calculate this value with systemClock and CLOCKS_PER_SECOND or F_CPU in mcuClock if necessary and subtract the sleep times
// to keep this implementation as close as possible to the ansi/iso 9899-1990. After implementation connect it to the version in time.h
uint32_t clock(void) {
    return -1;
}

//time1 - time0
int32_t difftime(uint32_t time1, uint32_t time0) {

    // 0 - uint32_t_max exceeds max negative values of int32_T, avoid the operation
    int64_t diff = (int64_t) time1 - (int64_t) time0;
    //return INT32_MIN if we exceed its boundary
    return (diff < INT32_MIN) ? INT32_MIN : (int32_t) diff;
}

// Converts the given year, month, day, hour, minute, and second into seconds since the epoch
uint32_t mktime(const struct tm * timeptr) {
    const struct tm time = (*timeptr);

    // Calculate number of days since the epoch
    uint16_t daysSinceEpoch = (time.tm_year - EPOCH_YEAR) * 365;

    // add one day for leap years
    for (uint16_t y = EPOCH_YEAR; y < time.tm_year; y++) {
        if (isLeapYear(y)) { daysSinceEpoch++; }
    }

    for (uint8_t m = 1; m < time.tm_mon; m++) { daysSinceEpoch += daysInMonth(time.tm_year, m); }

    uint32_t secondsSinceEpoch = daysSinceEpoch * ONE_DAY;
    secondsSinceEpoch += (time.tm_mday - 1) * ONE_DAY;
    secondsSinceEpoch += time.tm_hour * ONE_HOUR;
    secondsSinceEpoch += time.tm_min * 60;
    secondsSinceEpoch += time.tm_sec;

    return secondsSinceEpoch;
}

uint8_t isLeapYear(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

uint32_t time(uint32_t * timer) {
    uint32_t timestamp = 0;

    if (takeTimeFromClock != NULL) {
        timestamp = takeTimeFromClock();
        if (timer != NULL) { (*timer) = timestamp; }
    } else if (timer != NULL) {
        timestamp = (*timer);
    }
    return timestamp;
}

char * ctime(uint32_t * timer) {
    uint32_t timestamp = time(timer);
    struct tm * timePointer = localtime(&timestamp);
    char * result = asctime(timePointer);
    free(timePointer);
    return result;
}

char * asctime(struct tm * timeptr) {
    char * result = (char *) malloc(TIMESTAMP_LENGTH * sizeof(char));
    if (result == NULL) { return NULL; }

    formatString(result, timeptr);
    switch (timeptr->tm_isdst) {
        case 1: {
            //CAVE: intended change of meanings for more efficient operations
            addTimezone(result, 2);
            break;
        }
        case 2: {
            addTimezone(result, 1);
            break;
        }
        case 0: {
            addTimezone(result, 0);
        }
        default:;
    }
    // null terminate the string
    result[25] = '\0';
    return result;
}

// CAVE: The meaning is now shifted for more efficient bit operation, flag 0 still UTC, but Flag 1 CEST now, Flag 2 CEST
void addTimezone(char * result, uint8_t timezoneFlag) {
    uint8_t adjustment = timezoneFlag & 1;
    uint8_t index = 20 - adjustment; //will shift the position on to the left for CEST

    // (
    result[index++] = 0x28;
    if (!timezoneFlag) {
        // U
        result[index++] = 0x55;
        result[index++] = 0x54;
        // C
        result[index++] = 0x43;
    } else {
        result[index++] = 0x43;
        result[index++] = 0x45;
        if (adjustment) {
            //S
            result[index++] = 0x53;
        }
        result[index++] = 0x54;
    }
    // )
    result[index++] = 0x29;
}

void formatString(char * resultString, struct tm * timeStructPtr) {
    AsciiHelper * helper = dOS_initAsciiHelper();
    if (helper != NULL) {
        helper->integerToAscii(resultString, timeStructPtr->tm_year, 4, 0);
        helper->integerToAscii(resultString, timeStructPtr->tm_mon, 2, 5);
        helper->integerToAscii(resultString, timeStructPtr->tm_mday, 2, 8);
        helper->integerToAscii(resultString, timeStructPtr->tm_hour, 2, 11);
        helper->integerToAscii(resultString, timeStructPtr->tm_min, 2, 14);
        helper->integerToAscii(resultString, timeStructPtr->tm_sec, 2, 17);
        // -
        resultString[4] = resultString[7] = 0x2d;
        //whitespace
        resultString[10] = 0x20;
        // :
        resultString[13] = resultString[16] = 0x3a;
        // whitespace
        resultString[19] = 0x20;
    }
    free(helper);
}

struct tm * gmtime(const uint32_t * timer) {

    struct tm * constructedTime = (struct tm *) malloc(sizeof(struct tm));
    if (constructedTime == NULL) { return NULL; }

    uint32_t timeValue = (*timer);

    constructedTime->tm_sec = timeValue % 60;
    timeValue /= 60;
    constructedTime->tm_min = timeValue % 60;
    timeValue /= 60;
    constructedTime->tm_hour = timeValue % 24;
    timeValue /= 24;
    constructedTime->tm_year = calcYear(&timeValue);
    constructedTime->tm_mon = calcMonth(&timeValue, constructedTime->tm_year);
    constructedTime->tm_mday = timeValue + 1; // Days start from 0, so add 1

    return constructedTime;
}

struct tm * localtime(const uint32_t * timer) {
    uint32_t adjusted = (*timer);
    uint8_t utcOffset = calcUtcOffset(adjusted);
    // Adjust for UTC offset
    adjusted += utcOffset * ONE_HOUR;
    struct tm * timeToReturn = gmtime(&adjusted);

    timeToReturn->tm_isdst = utcOffset;
    return timeToReturn;
}

size_t strftime(char * s, size_t maxsize, const char * format, const struct tm * timeptr) {
    return 0;
}

//time1 - time0
uint32_t difftime_unsigned(uint32_t time1, uint32_t time0) {

    int64_t diff = (int64_t) time1 - (int64_t) time0;
    return (uint32_t) (diff >= 0 ? diff : -diff);
}

void setMcuClockCallback(uint32_t (* mcuClockCallback)(void)) {
    takeTimeFromClock = mcuClockCallback;
}

// Returns the number of days in a given month of a given year
uint8_t daysInMonth(uint16_t year, uint8_t month) {

    static const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t daysInMonth = days[month - 1];

    if (month == 2 && isLeapYear(year)) { daysInMonth++; }

    return daysInMonth;
}

uint8_t calcZellerCongruence(uint16_t year, uint8_t month, uint8_t day) {
    uint32_t h;
    if (month == 1) {
        month = 13;
        year--;
    }
    if (month == 2) {
        month = 14;
        year--;
    }
    h = day + 13 * (month + 1) / 5 + (year % 100) + (year % 100) / 4 + (year / 100) / 4 + 5 * (year / 100);
    h = h % 7;
    return h;
}

uint8_t isDst(uint16_t year, uint8_t month, uint8_t day) {
    if (month > 3 && month < 10) { return 1; }
    if (month < 3 || month > 10) { return 0; }

    uint8_t lastDayOfMonth = calcZellerCongruence(year, month, 31);
    uint8_t lastSunday = 31 - ((lastDayOfMonth - 1) % 7);

    if ((day >= lastSunday && month == 3) || (day < lastSunday && month == 10)) { return 1; }
    return 0;
}

uint16_t calcYear(uint32_t * days) {
    uint16_t year = EPOCH_YEAR;
    while ((*days) >= (uint16_t) 365 + isLeapYear(year)) {
        if (isLeapYear(year)) {
            (*days) -= 366;
            year++;
        } else {
            (*days) -= 365;
            year++;
        }
    }
    return year;
}

uint8_t calcMonth(uint32_t * days, uint16_t year) {
    uint8_t month = 1;
    while ((*days) >= daysInMonth(year, month)) {
        (*days) -= daysInMonth(year, month);
        month++;
    }
    return month;
}

uint8_t calcUtcOffset(uint32_t epochTimeY2K) {
    uint32_t days = epochTimeY2K /= ONE_DAY;
    uint16_t year = calcYear(&days);
    uint8_t month = calcMonth(&days, year);
    // Days start from 0, so add 1
    uint8_t day = days + 1;
    return isDst(year, month, day) ? 2 : 1;
}