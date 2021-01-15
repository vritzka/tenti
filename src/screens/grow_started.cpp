#include "grow_started.h"
#include "icons.h"

void GrowStartedScreen::render()
{
    tft.fillScreen(ILI9341_OLIVE);

    tft.drawBitmap(184, 160, plant_filled_72x72, 72, 72, ILI9341_WHITE);
}

void GrowStartedScreen::update()
{
    // override to prevent dimmer overlays
}