#include "tent.h"
#include "screen_manager.h"
#include <ArduinoJson.h>

extern ScreenManager screenManager;

const int minBrightness = 20;

Tent::Tent()
    : sensorTimer { Timer(5000, &Tent::markNeedsSensorUpdate, *this) }
    , minuteTimer { Timer(60000, &Tent::minutelyTick, *this) }
    , displayDimTimer { Timer(120000, &Tent::displayLightLow, *this, true) }
    , displayOffTimer { Timer(300000, &Tent::displayLightOff, *this, true) }
{
    this->growLightStatus = "OFF";
}

void Tent::setup()
{
    Particle.variable("tentTemperatureC", sensors.tentTemperatureC);
    Particle.variable("tentTemperatureF", sensors.tentTemperatureF);
    Particle.variable("tentHumidity", sensors.tentHumidity);
    Particle.variable("soilTemperatureC", sensors.soilTemperatureC);
    Particle.variable("soilTemperatureF", sensors.soilTemperatureF);
    Particle.variable("soilMoisture", rawSensors.soilMoisture);
    Particle.variable("waterLevel", sensors.waterLevel);

    // init sensors
    sht20.initSHT20();
    delay(255);
    sht20.checkSHT20();
    sht30.setAddress(0);
    sht30.update();
    soil.begin();

    displayLightHigh();

    // was there a grow/dry in process before (re)booting?
    if (state.getDayCount() > -1) {

        if (state.getVersion() != 2) {
            state.migrate();
        }

        if (state.isDay()) { // was the light on when we restarted?
            growLight("HIGH");
        }

        start();

    } else {
        state.init();
    }

    state.begin();

    if (state.getWifiStatus() && WiFi.hasCredentials())
        Particle.connect();
}

void Tent::start()
{
    minuteTimer.start();
    sensorTimer.start();

    fan("ON");
    markNeedsSensorUpdate();
    countMinute();
}

void Tent::stop()
{
    minuteTimer.stop();
    sensorTimer.stop();
}

void Tent::checkTent()
{
    double rawTemp = sht20.readTemperature();
    if (rawTemp == 998.0) {
        bool updated = sht30.update();

        if (!updated || sht30.temperature > 900) {

            if (sensors.tentTemperatureC != -1) {
                sensors.tentTemperatureC = sensors.tentTemperatureF = -1;
                screenManager.markNeedsRedraw(TEMPERATURE);
            }

            if (sensors.tentHumidity != -1) {
                sensors.tentHumidity = -1;
                screenManager.markNeedsRedraw(HUMIDITY);
            }

            rawSensors.tentTemperature = -1;
            rawSensors.tentHumidity = -1;
            return;
        }

        rawSensors.tentTemperature = sht30.temperature;
        rawSensors.tentHumidity = sht30.humidity;

    } else {
        rawSensors.tentTemperature = rawTemp;
        rawSensors.tentHumidity = sht20.readHumidity();
    }

    double currentTemp = (int)(rawSensors.tentTemperature * 10) / 10.0;
    double currentHumidity = (int)(rawSensors.tentHumidity * 10) / 10.0;
    Serial.printlnf("action=sensor name=tent humidity=%.1f temperature=%.1f", currentHumidity, currentTemp);

    if ((sensors.tentTemperatureC == 0) || (sensors.tentTemperatureC != currentTemp)) {
        sensors.tentTemperatureC = currentTemp;
        sensors.tentTemperatureF = (currentTemp == 0 || currentTemp > 900) ? currentTemp : (currentTemp * 1.8 + 32);
        screenManager.markNeedsRedraw(TEMPERATURE);
    }

    if ((sensors.tentHumidity == 0) || (sensors.tentHumidity != currentHumidity)) {
        sensors.tentHumidity = currentHumidity;
        screenManager.markNeedsRedraw(HUMIDITY);
    }
}

void Tent::checkSoil()
{
    double temp = soil.getTemperature() / 10.0;
    int moisture = soil.getCapacitance();
    if (moisture == 65535 || temp == -0.1) {
        if (sensors.soilTemperatureC != -1) {
            sensors.soilTemperatureC = sensors.soilTemperatureF = -1;
            screenManager.markNeedsRedraw(SOIL_TEMPERATURE);
        }
        if (sensors.waterLevel != 0) {
            sensors.waterLevel = 0;
            screenManager.markNeedsRedraw(SOIL_MOISTURE);
        }
        rawSensors.soilTemperature = -1;
        rawSensors.soilMoisture = -1;
        return;
    }

    double waterLevel = (moisture - 275.0) * 100.0 / (575.0 - 275.0);
    waterLevel = (int)(waterLevel * 10) / 10.0;
    if (waterLevel > 100) {
        waterLevel = 100;
    } else if (waterLevel < 0) {
        waterLevel = 0;
    }
    Serial.printlnf("action=sensor name=soil moisture=%d temperature=%.1f", moisture, temp);

    rawSensors.soilMoisture = moisture;
    rawSensors.soilTemperature = temp;

    if ((sensors.waterLevel == 0) || (sensors.waterLevel != waterLevel)) {
        sensors.waterLevel = waterLevel;
        screenManager.markNeedsRedraw(SOIL_MOISTURE);
    }

    if ((sensors.soilTemperatureC == 0) || (sensors.soilTemperatureC != temp)) {
        sensors.soilTemperatureC = temp;
        sensors.soilTemperatureF = (temp == 0 || temp > 900) ? temp : (temp * 1.8 + 32);
        screenManager.markNeedsRedraw(SOIL_TEMPERATURE);
    }
}

void Tent::fan(String fanStatus)
{
    if (fanStatus == "OFF") {
        analogWrite(FAN_SPEED_PIN, 255, 25000);
        analogWrite(FAN_SPEED_OPTICAL_PIN, 0, 10000);
    } else {
        int fanSpeed = map(state.getFanSpeed(), 0.0, 100.0, 0.0, 255.0);
        analogWrite(FAN_SPEED_PIN, 255 - fanSpeed, 25000);
        analogWrite(FAN_SPEED_OPTICAL_PIN, fanSpeed, 10000);
    }
}

void Tent::markNeedsSensorUpdate()
{
    needsSensorUpdate = true;
}

void Tent::checkSensors()
{
    if (!needsSensorUpdate) {
        return;
    }
    needsSensorUpdate = false;

    checkTent();
    checkSoil();
    adjustFan();
}

void Tent::checkInputs()
{
    if (digitalRead(DIM_PIN) == LOW) {
        displayLightHigh();
        dimmerBtnPressed = true;
        return;
    } else if (!dimmerBtnPressed) {
        return;
    }

    unsigned long now = millis();
    unsigned long diff = now - lastDimmerBtnTime;

    if (diff <= 300)
        return;

    lastDimmerBtnTime = now;
    dimmerBtnPressed = false;

    if (diff <= 600 && growLightStatus == "LOW") {
        muteGrowLight();
    } else {
        dimGrowLight();
    }
}

void Tent::minutelyTick()
{
    if (this->growLightStatus == "LOW") {
        this->dimTimeout -= 1;
        if (this->dimTimeout == 0) {
            this->growLight("HIGH");
        }
        screenManager.markNeedsRedraw(DIMMED);
    }

    countMinute();
    publishMetrics();
}

void Tent::publishMetrics()
{
    if (!Particle.connected())
        return;

    const size_t capa = JSON_OBJECT_SIZE(12);
    DynamicJsonDocument json(capa);
    json["time"] = Time.now();
    if (rawSensors.tentTemperature != -1) {
        json["air_temperature"] = rawSensors.tentTemperature;
    }
    if (rawSensors.tentHumidity != -1) {
        json["air_humidity"] = rawSensors.tentHumidity;
    }
    if (rawSensors.soilMoisture != -1) {
        json["soil_capacitance"] = rawSensors.soilMoisture;
        json["soil_moisture"] = sensors.waterLevel;
    }
    if (rawSensors.soilTemperature != -1) {
        json["soil_temperature"] = rawSensors.soilTemperature;
    }
    if (state.getDayCount() != -1) {
        json["grow_days"] = state.getDayCount();
        json["light_on"] = rawSensors.lightBrightness;
        json["light_period"] = state.getDayDuration();
        json["period_progress"] = state.getMinutesInPhotoperiod();
        json["fan_auto"] = state.getFanAutoMode() ? 1 : 0;
        json["fan_speed"] = state.getFanSpeed();
    }

    String data;
    serializeJson(json, data);
    Particle.publish("metrics", data, PRIVATE);
}

int Tent::growLight(String brightness)
{
    int maxBrightness = map(state.getLedBrightnessMax(), 0, 133, 0, 255); //the dimmer hardware produces 10V @ about 70%
    if (brightness == "HIGH") {
        analogWrite(GROW_LIGHT_BRIGHTNESS_PIN, maxBrightness, 25000);
        digitalWrite(GROW_LIGHT_ON_OFF_PIN, HIGH);
        growLightStatus = brightness;
        float brightnessForSensor = state.getLedBrightnessMax();
        brightnessForSensor /= 100;
        rawSensors.lightBrightness = brightnessForSensor;

    } else if (brightness == "LOW") {
        int maxBrightness = map(state.getLedBrightnessMax(), 0, 133, 0, 255);
        if (minBrightness < maxBrightness) {
            analogWrite(GROW_LIGHT_BRIGHTNESS_PIN, minBrightness, 25000);
            digitalWrite(GROW_LIGHT_ON_OFF_PIN, HIGH);
        }
        growLightStatus = brightness;
        dimTimeout = 15;
        rawSensors.lightBrightness = 0.1;

    } else if (brightness == "MUTE") {
        digitalWrite(GROW_LIGHT_ON_OFF_PIN, LOW);
        growLightStatus = brightness;
        rawSensors.lightBrightness = 0.0;

    } else if (brightness == "OFF") {
        digitalWrite(GROW_LIGHT_ON_OFF_PIN, LOW);
        growLightStatus = brightness;
        rawSensors.lightBrightness = 0.0;
    }

    return 1;
}

String Tent::getGrowLightStatus()
{
    return this->growLightStatus;
}

void Tent::fadeGrowLight(String mode, int percent)
{
    int maxBrightness = map(state.getLedBrightnessMax(), 0, 133, 0, 255);
    int brightnessRange = maxBrightness - minBrightness;
    int brightness = maxBrightness / 2;
    if (mode == "SUNRISE") {
        brightness = minBrightness + (brightnessRange * percent / 100);
    } else if (mode == "SUNSET") {
        brightness = maxBrightness - (brightnessRange * percent / 100);
    }
    analogWrite(GROW_LIGHT_BRIGHTNESS_PIN, brightness, 25000);
    digitalWrite(GROW_LIGHT_ON_OFF_PIN, HIGH);
    rawSensors.lightBrightness = brightness / 255.0;
}

void Tent::dimGrowLight()
{
    if (growLightStatus == "HIGH") {
        growLight("LOW");
    } else if (growLightStatus == "LOW" || growLightStatus == "MUTE") {
        growLight("HIGH");
    }

    screenManager.markNeedsRedraw(DIMMED);
}

void Tent::muteGrowLight()
{
    if (growLightStatus != "MUTE") {
        growLight("MUTE");
    }

    screenManager.markNeedsRedraw(DIMMED);
}

bool Tent::displayLightHigh()
{
    unsigned long now = millis();

    if ((now - lastDisplayLightTime) >= 15000 || lastDisplayLightTime == 0) {
        lastDisplayLightTime = now;

        while (displayBrightness < 255) {
            displayBrightness += 5;
            analogWrite(TFT_BRIGHTNESS_PIN, displayBrightness);

            delay(5);
        }
        RGB.brightness(255);
        RGB.control(false);

        displayDimTimer.start();
        displayOffTimer.start();
        return true;

    } else {
        return false;
    }
}

void Tent::displayLightLow(void)
{
    while (displayBrightness > 30) {
        displayBrightness -= 5;
        analogWrite(TFT_BRIGHTNESS_PIN, displayBrightness);
        delay(10);
    }
}

void Tent::displayLightOff(void)
{
    analogWrite(TFT_BRIGHTNESS_PIN, 0);
    displayBrightness = 0;
    RGB.control(true);
    RGB.brightness(0);
}

int16_t Tent::getDisplayBrightness(void)
{
    return displayBrightness;
}

void Tent::countMinute(bool ignoreDayCounter)
{
    state.setMinutesInPhotoperiod(state.getMinutesInPhotoperiod() + 1);

    int minutesInPeriod = state.getMinutesInPhotoperiod();
    int dayDuration = state.getDayDuration();
    int nightDuration = (24 * 60) - dayDuration;

    if (state.isDay()) {
        if (minutesInPeriod > dayDuration) { //day is over
            growLight("OFF");
            state.setIsDay(false);
            state.setMinutesInPhotoperiod(0);
            screenManager.markNeedsRedraw(DAY);

        } else if (minutesInPeriod >= dayDuration - 10) { //day is ending
            if (growLightStatus == "HIGH") {
                fadeGrowLight("SUNSET", (10 - (dayDuration - minutesInPeriod)) * 10);
            }
        }

    } else {
        if (state.getMode() == 'g') {
            if (minutesInPeriod > nightDuration) { //night is over (growing)
                if (!ignoreDayCounter)
                    state.setDayCount(state.getDayCount() + 1);
                growLight("HIGH");
                state.setIsDay(true);
                state.setMinutesInPhotoperiod(0);
                screenManager.markNeedsRedraw(DAY);

            } else if (minutesInPeriod >= nightDuration - 10) { //night is ending
                if (growLightStatus == "OFF") {
                    fadeGrowLight("SUNRISE", (10 - (nightDuration - minutesInPeriod)) * 10);
                }
            }
        } else {
            if (minutesInPeriod > nightDuration) { //night is over (drying)
                state.setDayCount(state.getDayCount() + 1);
                state.setMinutesInPhotoperiod(0);
                screenManager.markNeedsRedraw(DAY);
            }
        }
    }

    screenManager.markNeedsRedraw(TIMER);
}

void Tent::adjustFan()
{
    if (!state.getFanAutoMode()) { //manual

        fan("ON");

    } else {
        float fanSpeedPercent;
        float fanReactTempLow;
        float fanReactTempHigh;
        char tempUnit = state.getTempUnit();
        float tentTemperature;
        float fanSpeedMinSetting = state.getFanSpeedMin();
        float fanSpeedMaxSetting = state.getFanSpeedMax();
        float targetTemperature = state.getTargetTemperature();
        float targetHumidity = state.getTargetHumidity();
        float fanReactHumLow;
        float fanReactHumHigh;
        float fanSpeedPercentbyHumidity;
        int8_t tempDiffinF = 4;
        int8_t humDiff = 5;

        if (tempUnit == 'F') {
            tentTemperature = sensors.tentTemperatureF;
            fanReactTempLow = targetTemperature - tempDiffinF;
            fanReactTempHigh = targetTemperature + tempDiffinF;

        } else if (tempUnit == 'C') {
            tentTemperature = sensors.tentTemperatureC;
            fanReactTempLow = targetTemperature - 2.22;
            fanReactTempHigh = targetTemperature + 2.22;
        }

        if (tentTemperature < fanReactTempLow) {
            fanSpeedPercent = fanSpeedMinSetting;

        } else if (tentTemperature >= fanReactTempLow && tentTemperature <= fanReactTempHigh) {
            fanSpeedPercent = map(tentTemperature, fanReactTempLow, fanReactTempHigh, fanSpeedMinSetting, fanSpeedMaxSetting);
            fanSpeedPercent = ceil(fanSpeedPercent);

        } else if (tentTemperature > fanReactTempHigh) {
            fanSpeedPercent = fanSpeedMaxSetting;
        }

        fanReactHumLow = targetHumidity - 2;
        fanReactHumHigh = targetHumidity + 3;

        if (sensors.tentHumidity < fanReactHumLow) {
            fanSpeedPercentbyHumidity = fanSpeedMinSetting;

        } else if (sensors.tentHumidity >= fanReactHumLow && sensors.tentHumidity <= fanReactHumHigh) {
            fanSpeedPercentbyHumidity = map(sensors.tentHumidity, fanReactHumLow, fanReactHumHigh, fanSpeedMinSetting, fanSpeedMaxSetting);
            fanSpeedPercentbyHumidity = ceil(fanSpeedPercentbyHumidity);

        } else if (sensors.tentHumidity > fanReactHumHigh) {
            fanSpeedPercentbyHumidity = fanSpeedMaxSetting;
        }

        if (fanSpeedPercentbyHumidity > fanSpeedPercent)
            fanSpeedPercent = fanSpeedPercentbyHumidity;

        if (fanSpeedPercent > fanSpeedMaxSetting)
            fanSpeedPercent = fanSpeedMaxSetting;

        //sensor fail
        if (sensors.tentHumidity < 0)
            fanSpeedPercent = fanSpeedMinSetting + 10;

        if (fanSpeedPercent != state.getFanSpeed()) {
            state.setFanSpeed(fanSpeedPercent);
            fan("ON");
            screenManager.markNeedsRedraw(FAN);
        }
    }
}

float Tent::convertFtoC(float tempF)
{
    return (tempF - 32) * 0.5556;
}

float Tent::convertCtoF(float tempC)
{
    return (tempC * 1.8) + 32;
}
