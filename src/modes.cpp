#include "modes.h"
#include "steppers.h"
#include "globals.h"
#include "time_utils.h"

int timeToHour = 0;
int timeToMinute = 0;
int timeToSecond = 0;
int lastTimeToHour = 0;
int lastTimeToMinute = 0;
int lastTimeToSecond = 0;

int timeToClock = 0;

void RunMode1()
{
    SetTimeSpeed(S1, M1, H1);
}

void MoveToTimeTo(int timeToClock)
{
    //______Get Time______
    GetLocalTimeSafe(timeinfo);

    lastUpdatedTimeData = millis();

    Serial.printf("Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    int targetHour = 12 - (timeToHour - timeinfo.tm_hour);
    int targetMinute = 60 - (timeToMinute - timeinfo.tm_min);
    int targetSecond = 60 - (timeToSecond - (timeinfo.tm_sec + adjust_hands_speed / 1000));
        

    switch (timeToClock)
    {
    case 0:
        
        clock1TargetPositions[0] = (int)(timeToHour * hourStepsPerHour + timeToMinute * hourStepsPerMinute + timeToSecond * hourStepsPerSecond) % stepsPerTurn;
        clock1TargetPositions[1] = (int)(timeToMinute * minuteStepsPerMinute + timeToSecond * minuteStepsPerSecond) % stepsPerTurn;
        clock1TargetPositions[2] = (int)(timeToSecond * secondStepsPerSecond) % stepsPerTurn;

        //Serial.println(clock1TargetPositions[0]);
        ////Serial.println(clock1TargetPositions[2]);

                
        //______move to right time______
        clock2TargetPositions[0] = StepsToMoveToRightHour(H2.currentPosition(), timeinfo.tm_hour, timeinfo.tm_min);
        clock2TargetPositions[1] = StepsToMoveToRightMinute(M2.currentPosition(), timeinfo.tm_min, timeinfo.tm_sec);
        clock2TargetPositions[2] = StepsToMoveToRightSecond(SensorS1.readAngle(), timeinfo.tm_sec);

        SetModeChangeSpeed(S1, M1, H1);
        //SetModeChangeSpeed(S2, M2, H2);

        Clock1.moveTo(clock1TargetPositions);
        //Clock2.moveTo(clock2TargetPositions);
        while (H1.distanceToGo() != 0 || M1.distanceToGo() != 0 || S1.distanceToGo() != 0)
        {
            Clock1.run();
            //Clock2.run();
        }
        lastHourHandMovement = millis();
        
        break;
    
    case 1:

        clock1TargetPositions[0] = StepsToMoveToRightHour(H2.currentPosition(), timeinfo.tm_hour, timeinfo.tm_min);
        clock1TargetPositions[1] = StepsToMoveToRightMinute(M2.currentPosition(), timeinfo.tm_min, timeinfo.tm_sec);
        clock1TargetPositions[2] = StepsToMoveToRightSecond(SensorS1.readAngle(), timeinfo.tm_sec);

        //______move to right time______
        clock2TargetPositions[0] = (int)(timeToHour * hourStepsPerHour + timeToMinute * hourStepsPerMinute + timeToSecond * hourStepsPerSecond) % stepsPerTurn;
        clock2TargetPositions[1] = (int)(timeToMinute * minuteStepsPerMinute + timeToSecond * minuteStepsPerSecond) % stepsPerTurn;
        clock2TargetPositions[2] = (int)(timeToSecond * secondStepsPerSecond) % stepsPerTurn;

        SetModeChangeSpeed(S1, M1, H1);
        //SetModeChangeSpeed(S2, M2, H2);

        Clock1.moveTo(clock1TargetPositions);
        //Clock2.moveTo(clock2TargetPositions);
        while (H1.distanceToGo() != 0 || M1.distanceToGo() != 0 || S1.distanceToGo() != 0)
        {
            Clock1.run();
            //Clock2.run();
        }
        lastHourHandMovement = millis();

        break;
    }
}
