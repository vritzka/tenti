#ifndef TENT_H
#define TENT_H

#include "Particle.h"
#include <Arduino.h>
#include "DFRobot_SHT20.h"
#include "tent_state.h"

#define GROW_LIGHT_BRIGHTNESS_PIN TX
#define GROW_LIGHT_ON_OFF_PIN D7
#define FAN_SPEED_PIN RX
#define FAN_SPEED_MIN 10 //percent
#define TFT_BRIGHTNESS_PIN WKP
#define DIM_PIN DAC

extern DFRobot_SHT20 sht20;
extern double temp;
extern double hum;
extern double waterLevel;

class Tent {
private:
    int displayBrightness = 0;
    String growLightStatus;

public:
    Tent();
    TentState state;
    unsigned long lastTime = 0;
    int dimTimeout = 0;
    Timer* tp;
    Timer* tp1;
    bool checkStats;

    void begin();
    void adjustFan();
    void countMinute();
    void checkTemperature();
    void checkHumidity();
    void checkWaterLevel();
    void fan(String fanStatus);
    void doCheckStats();
    bool getCheckStats();
    void resetCheckStats();
    void minutelyTick();
    int growLight(String brightness);
    String getGrowLightStatus();
    void dimGrowLight();
    void displayLightLow(void);
    void displayLightOff(void);
    bool displayLightHigh(void);
};
#endif
