#ifndef FIRMWAREUPDATESCREEN_H
#define FIRMWAREUPDATESCREEN_H

#include "../screen.h"

class FirmwareUpdateScreen : public Screen {
public:
    String getName() { return "firmwareUpdateScreen"; }
    void render();
    void renderButton(Button& btn) {}
    void renderButtonPressed(Button& btn) {}
    void handleButton(Button& btn) {}
    void update();
};
#endif
