#ifndef MYSTRPTIME_H_
#define MYSTRPTIME_H_

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define ALT_E           0x01
#define ALT_O           0x02
#define LEGAL_ALT(x)        { if (alt_format & ~(x)) return (0); }
#define TM_YEAR_BASE 1900

char *mystrptime(const char *buf, const char *fmt, struct tm *tm);
time_t to_seconds(const char *date,int mode);

#endif