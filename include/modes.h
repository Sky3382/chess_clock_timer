#ifndef MODES_H
#define MODES_H

extern int timeToHour;
extern int timeToMinute;
extern int timeToSecond;
extern int lastTimeToHour;
extern int lastTimeToMinute;
extern int lastTimeToSecond;

extern int timeToClock;

void RunMode1();
void MoveToTimeTo(int timeToClock);

#endif // MODES_H