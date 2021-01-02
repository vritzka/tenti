#include "climate.h"
#include "icons.h"
#include "tent.h"
#include "screen_manager.h"

extern ScreenManager screenManager;
extern Tent tent;

void ClimateScreen::render()
{
    tft.fillScreen(ILI9341_BLACK);

    tft.drawBitmap(20, 4, thermometer_36, 36, 36, ILI9341_WHITE);

    tft.setCursor(60, 11);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.print("Climate Settings");
    
    tft.setCursor(64,65);
    tft.setTextSize(1);
    tft.print("Target Temperature");
    buttons.push_back(Button("targetTempUpBtn", 165, 86, 30, 30, "", 0, 0));
    buttons.push_back(Button("targetTempDownBtn", 40, 86, 30, 30, "", 0, 0));
        
    tft.setCursor(73,150);
    tft.setTextSize(1);
    tft.print("Target Humidity");
    buttons.push_back(Button("targetHumUpBtn", 165, 170, 30, 30, "", 0, 0));
    buttons.push_back(Button("targetHumDownBtn", 40, 170, 30, 30, "", 0, 0));    
    
    tft.setCursor(258, 55);
    tft.print("Unit");
    buttons.push_back(Button("climateUnitFBtn", 250, 70, 40, 38, "F", 15, 11));
    buttons.push_back(Button("climateUnitCBtn", 250, 107, 40, 38, "C", 15, 11));
    
    buttons.push_back(Button("climateOkBtn", 250, 180, 40, 38, "OK", 9, 12));

    drawTargetTemperature();
    drawTargetHumidity();

    renderButtons(true);
}

void ClimateScreen::renderButton(Button& btn)
{
    if (btn.getName() == "climateUnitFBtn") {
        drawButton(btn, tent.state.getTempUnit() == 'F' ? ILI9341_OLIVE : ILI9341_BLACK, 2);

    } else if (btn.getName() == "climateUnitCBtn") {
        drawButton(btn, tent.state.getTempUnit() == 'C' ? ILI9341_OLIVE : ILI9341_BLACK, 2);
        
    } else if (btn.getName() == "targetTempUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_RED);

    } else if (btn.getName() == "targetTempDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_RED);
        
    } else if (btn.getName() == "targetHumUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_RED);

    } else if (btn.getName() == "targetHumDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_RED);
        
    } else if (btn.getName() == "climateOkBtn") {
        drawButton(btn, ILI9341_OLIVE, 2);
        
    }
}

void ClimateScreen::renderButtonPressed(Button& btn)
{
    if (btn.getName() == "targetTempUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_WHITE);

    } else if (btn.getName() == "targetTempDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_WHITE);
        
    } else if (btn.getName() == "targetHumUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_WHITE);

    } else if (btn.getName() == "targetHumDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_WHITE);
        
    }
}

void ClimateScreen::handleButton(Button& btn)
{
    if (btn.getName() == "climateUnitFBtn") {
        tent.state.setTempUnit('F');
        renderButton(buttons[4]);
        renderButton(buttons[5]);
        drawTargetTemperature();

    } else if (btn.getName() == "climateUnitCBtn") {
        tent.state.setTempUnit('C');
        renderButton(buttons[4]);
        renderButton(buttons[5]);
        drawTargetTemperature();
        
    } else if(btn.getName() == "climateOkBtn") {
        screenManager.homeScreen();
        
    } else if(btn.getName() == "targetTempUpBtn") {
        float targetTemperature = tent.state.getTargetTemperature();
        if(targetTemperature < 95) {
            targetTemperature += 2;
            tent.state.setTargetTemperature(targetTemperature);
            drawTargetTemperature();
            tent.adjustFan();
        } else {
            drawTargetTemperature(true);  
        }
        
    } else if(btn.getName() == "targetTempDownBtn") {
        float targetTemperature = tent.state.getTargetTemperature();
        if(targetTemperature > 49) {
            targetTemperature -= 2;
            tent.state.setTargetTemperature(targetTemperature);
            drawTargetTemperature();
            tent.adjustFan();
        } else {
            drawTargetTemperature(true);  
        }
        
    } else if(btn.getName() == "targetHumUpBtn") {
        float targetHumidity = tent.state.getTargetHumidity();
        if(targetHumidity < 95) {
            targetHumidity += 5;
            tent.state.setTargetHumidity(targetHumidity);
            drawTargetHumidity();
            tent.adjustFan();
        } else {
            drawTargetHumidity(true);  
        }
        
    } else if(btn.getName() == "targetHumDownBtn") {
        float targetHumidity = tent.state.getTargetHumidity();
        if(targetHumidity > 5) {
            targetHumidity -= 5;
            tent.state.setTargetHumidity(targetHumidity);
            drawTargetHumidity();
            tent.adjustFan();
        } else {
            drawTargetHumidity(true);  
        }
        
    }
}

void ClimateScreen::drawTargetTemperature(bool warning)
{
    tft.fillRect(92, 93, 57, 15, ILI9341_BLACK);
    float targetTemperature = tent.state.getTargetTemperature();
    char tempUnit = tent.state.getTempUnit();
    
    if(tempUnit == 'C') {
        targetTemperature = tent.convertFtoC(targetTemperature);
    }
    
    if(warning) {
        tft.setCursor(92, 93);
        tft.setTextSize(2);
        tft.setTextColor(ILI9341_RED);
        tft.print(String::format("%.1f", targetTemperature));
        tft.setTextSize(1);
        tft.print(tempUnit);
        delay(50);
    }
    tft.setCursor(92, 93);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(String::format("%.1f", targetTemperature));
    tft.setTextSize(1);
    tft.print(tempUnit);
    
    
}

void ClimateScreen::drawTargetHumidity(bool warning)
{
    tft.fillRect(92, 177, 57, 15, ILI9341_BLACK);
    float targetHumidity = tent.state.getTargetHumidity();
    if(warning) {
        tft.setTextColor(ILI9341_RED);
        if(targetHumidity < 10) {
            tft.setCursor(97, 177);
        } else {
            tft.setCursor(92, 177);
        }
        tft.setTextSize(2);
        tft.print(String::format("%.1f", targetHumidity));
        tft.setTextSize(1);
        tft.print("%");
        delay(50);
    }
    tft.setTextColor(ILI9341_WHITE);
    if(targetHumidity < 10) {
        tft.setCursor(97, 177);
    } else {
        tft.setCursor(92, 177);
    }
    tft.setTextSize(2);
    tft.print(String::format("%.1f", targetHumidity));
    tft.setTextSize(1);
    tft.print("%");
    
}