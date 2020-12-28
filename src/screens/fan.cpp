#include "fan.h"
#include "icons.h"
#include "tent.h"
#include "screen_manager.h"

extern ScreenManager screenManager;
extern Tent tent;

void FanScreen::render()
{
    tft.fillScreen(ILI9341_BLACK);

    tft.drawBitmap(20, 4, fan_36, 36, 36, ILI9341_WHITE);

    tft.setCursor(60, 8);
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);
    tft.print("Speed");

    buttons.push_back(Button("fanAutoBtn", 20, 65, 150, 38, "Automatic", 20, 11));
    buttons.push_back(Button("fanManualBtn", 20, 115, 150, 38, "Manual", 40, 11));
    buttons.push_back(Button("fanOkBtn", 20, 170, 150, 38, "Ok", 55, 8));
    if(!tent.state.getFanAutoMode()) {
        buttons.push_back(Button("fanUpBtn", 220, 75, 50, 50, "", 0, 0));
        buttons.push_back(Button("fanDownBtn", 220, 145, 50, 50, "", 0, 0));
    } else {
        buttons.push_back(Button("climateGoalsBtn", 220, 75, 50, 50, "", 0, 0));
    }

    renderButtons(true); 
    drawFanStatus();
}

void FanScreen::update()
{
    if (screenManager.wasNeedsRedraw(FAN))
        drawFanStatus();

    Screen::update();
}

void FanScreen::renderButton(Button& btn)
{
    if (btn.getName() == "fanAutoBtn") {
        drawButton(btn, tent.state.getFanAutoMode() ? ILI9341_OLIVE : ILI9341_BLACK, 2);

    } else if (btn.getName() == "fanManualBtn") {
        drawButton(btn, !tent.state.getFanAutoMode() ? ILI9341_OLIVE : ILI9341_BLACK, 2);

    } else if (btn.getName() == "fanUpBtn") {
        if(tent.state.getFanAutoMode())
            return;
        drawButtonTriangleUp(btn, ILI9341_RED);

    } else if (btn.getName() == "fanDownBtn") {
        if(tent.state.getFanAutoMode())
            return;
        drawButtonTriangleDown(btn, ILI9341_RED);

    } else if (btn.getName() == "fanOkBtn") {
        drawButton(btn, ILI9341_OLIVE, 3);
        
    } else if (btn.getName() == "climateGoalsBtn") {
        
        tft.drawRect(200, 60, 90, 130, ILI9341_DARKGREY);
        
        tft.setTextColor(ILI9341_LIGHTGREY);
        
        tft.setCursor(200,75);
        tft.setTextSize(1);
        tft.print("Fan Speed Minimum");
        tft.setCursor(200,85);
        tft.setTextSize(2);
        tft.print(String::format("%.1f", tent.state.getFanSpeedMin()) + "%");
        tft.setCursor(200,95);
        tft.setTextSize(1);
        tft.println("Fan Speed Maximum:");
        tft.setCursor(200,115);
        tft.setTextSize(2);
        tft.print(String::format("%.1f", tent.state.getFanSpeedMax()) + "%");
        
    }
}

void FanScreen::renderButtonPressed(Button& btn)
{
    if (btn.getName() == "fanUpBtn") {
        drawButtonTriangleUp(btn, ILI9341_WHITE);

    } else if (btn.getName() == "fanDownBtn") {
        drawButtonTriangleDown(btn, ILI9341_WHITE);
    }
}

void FanScreen::handleButton(Button& btn)
{
    if (btn.getName() == "fanAutoBtn") {
        
        if(tent.state.getFanAutoMode())
            return;
            
        tent.state.setFanAutoMode(true);
        
        tft.fillRect(220,75,55,122,ILI9341_BLACK);
        buttons.pop_back();
        buttons.pop_back();
        
        buttons.push_back(Button("climateGoalsBtn", 220, 75, 50, 50, "", 0, 0));

        renderButton(buttons[0]);
        renderButton(buttons[1]);
        renderButton(buttons[3]);
        
        tent.adjustFan();
        screenManager.markNeedsRedraw(FAN);

    } else if (btn.getName() == "fanManualBtn") {
        if(!tent.state.getFanAutoMode())
            return;
        tent.state.setFanAutoMode(false);
        
        tft.fillRect(210,60,55,122,ILI9341_BLUE);
        buttons.pop_back();
        buttons.push_back(Button("fanUpBtn", 220, 75, 50, 50, "", 0, 0));
        buttons.push_back(Button("fanDownBtn", 220, 145, 50, 50, "", 0, 0));

        renderButton(buttons[0]);
        renderButton(buttons[1]);
        renderButton(buttons[3]);
        renderButton(buttons[4]); 
        
        tent.adjustFan();
        screenManager.markNeedsRedraw(FAN);

    } else if (btn.getName() == "fanUpBtn") {
        float fanSpeedPercent = tent.state.getFanSpeed();
        tent.state.setFanAutoMode(false);
        renderButton(buttons[0]);
        renderButton(buttons[1]);

        if (fanSpeedPercent < 100) {
            fanSpeedPercent += 5;
            tent.state.setFanSpeed(fanSpeedPercent);
            tent.adjustFan();
            screenManager.markNeedsRedraw(FAN);
        }

    } else if (btn.getName() == "fanDownBtn") {
        float fanSpeedPercent = tent.state.getFanSpeed();
        tent.state.setFanAutoMode(false);
        renderButton(buttons[0]);
        renderButton(buttons[1]);

        if (fanSpeedPercent > 5) {
            fanSpeedPercent -= 5;
            tent.state.setFanSpeed(fanSpeedPercent);
            tent.adjustFan();
            screenManager.markNeedsRedraw(FAN);
        }

    } else if (btn.getName() == "fanOkBtn") {
        screenManager.homeScreen();
    }
}
