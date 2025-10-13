#ifndef CLOCKS_H
#define CLOCKS_H

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <AS5600.h>
#include <TMCStepper.h>
#include <Wire.h>

#define TIME_FOR_GOING_TO_TIME_POSITION 14000

// ======= DIGITAL HALL SENSORS =======
#define HALL_10_PIN 41
#define HALL_11_PIN 42
#define HALL_20_PIN 45
#define HALL_21_PIN 46

// ======= I2C HALL (AS5600) =======
extern AS5600 SensorS1;
#define HALL_12_SDA 17
#define HALL_12_SCL 18

extern AS5600 SensorS2;
#define HALL_22_SDA 19
#define HALL_22_SCL 20  

// ======= SERVOS =======
#define SERVO_1 26
#define SERVO_2 27

// ======= STEPPERS =======
// Clock 1 (Hour, Minute, Second)
#define H1_IN1 6
#define H1_IN2 7
#define H1_IN3 15
#define H1_IN4 16

#define M1_IN1 12
#define M1_IN2 13
#define M1_IN3 14
#define M1_IN4 21

#define S1_STEP 2
#define S1_DIR 1

// Clock 2 (Hour, Minute, Second)
#define H2_IN1 33
#define H2_IN2 34
#define H2_IN3 35
#define H2_IN4 36

#define M2_IN1 37
#define M2_IN2 38
#define M2_IN3 39
#define M2_IN4 40

#define S2_STEP 4
#define S2_DIR 5

// ======= CLOCK CLASS =======
class Clock
{
public:
    Clock(int clockNumber,
          int in1H, int in2H, int in3H, int in4H, int sensorH,
          int in1M, int in2M, int in3M, int in4M, int sensorM,
          int STEPS, int DIRS, AS5600& sensorS, int servoPin);

    void begin();
    void home();

    void moveToTime(int hour, int minute, int second);
    void setTimeSpeed();
    void setModeChangeSpeed();
    void runTimeMode();
    void runMultiMode();
    void normalizeStepCounts();
    void calibrateSecondHand();
    void getAndMoveToTime();

    void setTargetPositions(long hourSteps, long minuteSteps, long secondSteps, int currentHPosition, int currentMPosition, int currentSPosition);
    void computeAndSetTargetsFromTime(int hour, int minute, int second, int currentHPosition, int currentMPosition, int currentSPosition);
    long *getTargetArray();

    void printStatus();

    AccelStepper H, M, S;
    MultiStepper multi;
    AS5600 &sensorS;

    bool sensorHState;
    bool sensorMState;

    bool isActive = true;

private:
    int clockNumber;
    int sensorHPin;
    int sensorMPin;
    int servoPin;

    long targetPositions[3]; // [H, M, S]

    const int HMstepsPerTurn = 20480;
    const int SstepsPerTurn = 20480 * 4;
    const double hourStepsPerHour = (double)HMstepsPerTurn / 12.0;
    const double hourStepsPerMinute = hourStepsPerHour / 60.0;
    const double hourStepsPerSecond = hourStepsPerMinute / 60.0;
    const double minuteStepsPerMinute = HMstepsPerTurn / 60.0;
    const double minuteStepsPerSecond = minuteStepsPerMinute / 60.0;
    const double secondStepsPerSecond = SstepsPerTurn / 60.0;

    int HMhomingSpeed = 1000;
    int HMmodeChangeSpeed = 1000;
    int HMmodeChangeAccel = 500;

    int ShomingSpeed = -4000;
    int SmodeChangeSpeed = -4000;
    int SmodeChangeAccel = 2000;

    int previousHMillis;

    int Normalize(int pos, int stepsPerTurn);
    int wrapTarget(int currentPosition, int target, int stepsPerTurn);

    void initSensors();
    void initSteppers();
};

extern Clock Clock1;
extern Clock Clock2;

#endif