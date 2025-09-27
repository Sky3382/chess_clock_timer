#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <time.h>

extern struct tm timeinfo;
void InitTime();
bool GetLocalTimeSafe(struct tm &timeinfo);

#endif // TIME_UTILS_H