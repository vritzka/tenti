#include "light.h"
#include "icons.h"
#include "tent.h"
#include "screen_manager.h"

extern ScreenManager screenManager;
extern Tent tent;

void LightScreen::render()
{
    tft.fillScreen(ILI9341_BLACK);

    uint16_t dayDuration = tent.state.getDayDuration();
    renderDayDuration(dayDuration);

    buttons.push_back(Button("timerUpBtn", 50, 50, 40, 40, "", 0, 0));
    buttons.push_back(Button("timerDownBtn", 50, 160, 40, 40, "", 0, 0));
    
    buttons.push_back(Button("brightnessUpBtn", 225, 50, 40, 40, "", 0, 0));
    buttons.push_back(Button("brightnessDownBtn", 225, 160, 40, 40, "", 0, 0));
    
    renderLedBrightness();
    drawTimerStatus();
    
    buttons.push_back(Button("lightOkBtn", 140, 180, 40, 38, "OK", 9, 12));

    renderButtons(true);
}

void LightScreen::update()
{
    if (screenManager.wasNeedsRedraw(TIMER))
        drawTimerStatus();
        
    Screen::update();
}


void LightScreen::renderButton(Button& btn)
{
    if (btn.getName() == "timerUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_RED);
        
    } else if (btn.getName() == "timerDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_RED);

    } else if (btn.getName() == "brightnessUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_RED);

    } else if (btn.getName() == "brightnessDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_RED);

    } else if (btn.getName() == "lightOkBtn") {
        drawButton(btn, ILI9341_OLIVE, 2);
    }
}

void LightScreen::renderButtonPressed(Button& btn)
{
    if (btn.getName() == "timerUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_WHITE);
        
    } else if (btn.getName() == "timerDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_WHITE);
        
    } else if (btn.getName() == "brightnessUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_WHITE);

    } else if (btn.getName() == "brightnessDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_WHITE);

    }
}

void LightScreen::handleButton(Button& btn)
{
    if (btn.getName() == "timerUpBtn") {
        int dayDuration = tent.state.getDayDuration() + 60;
        if (dayDuration > 1440) {
            dayDuration = 60;
        }
        tent.state.setDayDuration(dayDuration);
        renderDayDuration(dayDuration);
        drawTimerStatus();

    } else if (btn.getName() == "timerDownBtn") {
        int dayDuration = tent.state.getDayDuration() - 60;
        if (dayDuration <= 0) {
            dayDuration = 1440;
        }
        tent.state.setDayDuration(dayDuration);
        renderDayDuration(dayDuration);
        drawTimerStatus();

    } else if(btn.getName() == "brightnessUpBtn") {
        int brightness = tent.state.getLedBrightnessMax();
        if(brightness == 100)
            return;
            
        brightness += 5;
        tent.state.setLedBrightnessMax(brightness);
        tent.growLight("HIGH");
        renderLedBrightness();
        drawTimerStatus();
        screenManager.markNeedsRedraw(DIMMED);

    } else if(btn.getName() == "brightnessDownBtn") {
        int brightness = tent.state.getLedBrightnessMax();
        if(brightness == 5)
            return;
            
        brightness -= 5;
        tent.state.setLedBrightnessMax(brightness);
        tent.growLight("HIGH");
        renderLedBrightness(); 
        drawTimerStatus();
        screenManager.markNeedsRedraw(DIMMED);

    } else if (btn.getName() == "lightOkBtn") {
        screenManager.homeScreen();
    }
}

void LightScreen::renderDayDuration(int dayDuration)
{
    int dayDurationHours = dayDuration / 60;
    
    tft.fillRect(51, 115, 39, 23, ILI9341_BLACK);
    
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    
    
    if(dayDurationHours < 10)
    {
        tft.setCursor(63, 116);
        
    } else if(dayDurationHours >= 20) 
    {
        tft.setCursor(54, 116);
        
    } else 
    {
        tft.setCursor(52, 116);    
    }
                 
    tft.print(String(dayDurationHours));
    
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_LIGHTGREY);
    tft.setCursor(44,99);
    tft.print("Lights On");
    tft.setCursor(55,147);
    tft.print("Hours");
    

}

void LightScreen::renderLedBrightness() {
    tft.fillRect(219, 116, 60, 26, ILI9341_BLACK);
    int brightnessPercent = tent.state.getLedBrightnessMax();
    
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    
    if(brightnessPercent == 100)
    {
        tft.setCursor(219, 116);
        
    } else if(brightnessPercent == 5) 
    {
        tft.setCursor(238, 116);
        
    } else {
       tft.setCursor(228, 116); 
    }

    tft.print(String(brightnessPercent));
    
    tft.setTextColor(ILI9341_LIGHTGREY);
    tft.setTextSize(1);
    
    tft.setCursor(216,99);
    tft.print("Brightness");
    
    tft.setCursor(240,142);
    tft.setTextSize(2);
    tft.print("%");

}