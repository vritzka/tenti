#include "drying_hint.h"
#include "icons.h"
#include "tent.h"
#include "screen_manager.h"
#include <Arduino.h>
extern "C" {
#include "libs/qrduino/qrencode.h"
}

extern ScreenManager screenManager;
extern Tent tent;

DryingHintScreen::DryingHintScreen()
    : Screen()
{
    if (strinbuf[0] == 0) {
        String url = "https://www.how-to-marijuana.com/curing-marijuana.html";
        strncpy((char*)strinbuf, url.c_str(), 245);
        qrencode();
    }
}

void DryingHintScreen::render()
{
    tft.fillScreen(ILI9341_PURPLE);

    tft.drawBitmap(20, 4, wind_36, 36, 36, ILI9341_YELLOW);

    tft.setCursor(65, 16);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);
    tft.print("About Drying");

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);

    tft.setCursor(10, 50);
    tft.print("Slow drying will make a good product.");
    tft.setCursor(10, 60);
    tft.print("We aim to dry as slow as possible but as ");
    tft.setCursor(10, 70);
    tft.print("fast as necessary (to avoid mold).");
    tft.setCursor(10, 90);
    tft.print("A good drying climate is 66% humidity");
    tft.setCursor(10, 100);
    tft.print("and 66F (18C) temperature.");
    tft.setCursor(10, 120);
    tft.print("Stop drying once the branches snap with an");
    tft.setCursor(10, 130);
    tft.print("audible 'crack' when bending.");
    tft.setCursor(10, 150);
    tft.print("Depending on where you live,");
    tft.setCursor(10, 160);
    tft.print("drying takes between 3-8 days.");
    tft.setCursor(10, 180);
    tft.print("Then its time for curing.");

    int x, y, xOffset = 230, yOffset = 140;
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE);

    tft.setCursor(xOffset, yOffset);
    tft.print("More info");
    for (y = 0; y < WD; y++) {
        for (x = 0; x < WD; x++) {
            int color = QRBIT(x, y) ? ILI9341_WHITE : ILI9341_BLACK;
            int x0 = xOffset + x * 2, y0 = yOffset + 12 + y * 2;
            tft.drawPixel(x0, y0, color);
            tft.drawPixel(x0 + 1, y0, color);
            tft.drawPixel(x0, y0 + 1, color);
            tft.drawPixel(x0 + 1, y0 + 1, color);
        }
    }

    buttons.push_back(Button("okBtn", 30, 195, 140, 38, "Ok", 55, 8));

    renderButtons(true);
}

void DryingHintScreen::renderButton(Button& btn)
{
    if (btn.getName() == "okBtn") {
        drawButton(btn, ILI9341_OLIVE, 3);
    }
}

void DryingHintScreen::renderButtonPressed(Button& btn)
{
}

void DryingHintScreen::handleButton(Button& btn)
{
    if (btn.getName() == "okBtn") {
        screenManager.homeScreen();
    }
}

void DryingHintScreen::update()
{
    // override to prevent dimmer overlays
}
