#pragma once

#include "Particle.h"
#include <Arduino.h>
#include "math.h"

class TentState {

/*
    struct {
        char mode; // g=growing, d=dryimg
        int dayCounter; // counting days the grow or dry was active. Starting from 1
        bool isDay; // true if the light is on
        int minutesInPhotoperiod; // how long has the system been in current photoperiod?  E.g. 31 minutes in NIGHT
        int dayDuration; // how long is the light on?
        bool fanAutoMode; // 1 for auto, 0 for manual
        float fanSpeed; // 0-100%
        char tempUnit; // F or C
        bool wifiStatus; // 1=on, 0=off
        float targetTemperature;
        float targetHumidity;
        float fanSpeedMin;
        float fanSpeedMax;
        int ledBrightnessMax;
    } eeprom;
 */   
    
    struct {
        char mode; // g=growing, d=dryimg
        int dayCounter; // counting days the grow or dry was active. Starting from 1
        bool isDay; // true if the light is on
        int minutesInPhotoperiod; // how long has the system been in current photoperiod?  E.g. 31 minutes in NIGHT
        int dayDuration; // how long is the light on?
        bool fanAutoMode; // 1 for auto, 0 for manual
        float fanSpeed; // 0-100%
        char tempUnit; // F or C
        bool wifiStatus; // 1=on, 0=off
    } eeprom;

public:
    TentState();

/*
    void init()
    {
        eeprom.mode = 'g';
        eeprom.dayCounter = -1;
        eeprom.isDay = true;
        eeprom.minutesInPhotoperiod = 0;
        eeprom.dayDuration = 18 * 60;
        eeprom.fanAutoMode = 1;
        eeprom.fanSpeed = 30;
        eeprom.tempUnit = 'F';
        eeprom.wifiStatus = 1;
        eeprom.targetTemperature = 77.0;
        eeprom.targetHumidity = 50.0;
        eeprom.fanSpeedMin = 15;
        eeprom.fanSpeedMax = 40;
        eeprom.ledBrightnessMax = 100;
        save();
    }
    */
    void init()
    {
        eeprom.mode = 'g';
        eeprom.dayCounter = -1;
        eeprom.isDay = true;
        eeprom.minutesInPhotoperiod = 0;
        eeprom.dayDuration = 18 * 60;
        eeprom.fanAutoMode = 1;
        eeprom.fanSpeed = 30;
        eeprom.tempUnit = 'F';
        eeprom.wifiStatus = 1;
        save();
    }
    
    void migrate()
    {
        //updating earlier versions
        if (getTempUnit() != 'F' && getTempUnit() != 'C') {
            setTempUnit('F');

        } else if (isnan(getFanSpeedMin())) {
            setFanSpeedMin(15);

        } else if (isnan(getFanSpeedMax())) {
            setFanSpeedMax(40);

        } else if (isnan(getTargetTemperature())) {
            setTargetTemperature(77.0);

        } else if (isnan(getTargetHumidity())) {
            setTargetHumidity(50.0);

        } else if (getMode() != 'g' && getMode() != 'd') {
            setMode('g');
            
        } else if (getLedBrightnessMax() < 0) {
            setLedBrightnessMax(100);
        }
    }
    void begin();
    void save();

    bool isDay(void);
    void setIsDay(bool);

    int getDayCount(void);
    void setDayCount(int);

    char getMode(void);
    void setMode(char);

    int getMinutesInPhotoperiod(void);
    void setMinutesInPhotoperiod(int);

    int getDayDuration(void);
    void setDayDuration(int);

    bool getFanAutoMode(void);
    void setFanAutoMode(bool);

    float getFanSpeed(void);
    void setFanSpeed(float);

    float getFanSpeedMin(void);
    void setFanSpeedMin(float);

    float getFanSpeedMax(void);
    void setFanSpeedMax(float);

    char getTempUnit(void);
    void setTempUnit(char);

    bool getWifiStatus(void);
    void setWifiStatus(bool);

    float getTargetTemperature(void);
    void setTargetTemperature(float);

    float getTargetHumidity(void);
    void setTargetHumidity(float);
    
    int getLedBrightnessMax(void);
    void setLedBrightnessMax(int);
};
