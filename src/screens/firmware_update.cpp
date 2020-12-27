#include "firmware_update.h"
#include "icons.h"

void FirmwareUpdateScreen::render()
{
    tft.fillScreen(ILI9341_PURPLE);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);

    tft.setCursor(40, 50);
    tft.print("Firmware Updating...");

    tft.drawBitmap(124, 160, plant_filled_72x72, 72, 72, ILI9341_WHITE);
}

void FirmwareUpdateScreen::update()
{
    // override to prevent dimmer overlays
}