#ifndef MODES_H
#define MODES_H

// mode timeTo variables
extern int timeTo_hour;
extern int timeTo_minute;
extern int timeTo_second;
extern int last_timeTo_hour;
extern int last_timeTo_minute;
extern int last_timeTo_second;

// mode pomodoro variables
extern int pomodoro_timer1_hour;
extern int pomodoro_timer1_minute;
extern int pomodoro_timer1_second;

extern int pomodoro_timer2_hour;
extern int pomodoro_timer2_minute;
extern int pomodoro_timer2_second;

extern int last_pomodoro_timer1_hour;
extern int last_pomodoro_timer1_minute;
extern int last_pomodoro_timer1_second;

extern int last_pomodoro_timer2_hour;
extern int last_pomodoro_timer2_minute;
extern int last_pomodoro_timer2_second;

extern int timeToClock;

void RunMode1();
void MoveToTimeTo(int timeToClock);

#endif // MODES_H