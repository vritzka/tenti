#ifndef LIGHTSCREEN_H
#define LIGHTSCREEN_H

#include "../screen.h"

class LightScreen : public Screen {
public:
    String getName() { return "lightScreen"; }
    void render();
    void renderButton(Button& btn);
    void renderButtonPressed(Button& btn);
    void handleButton(Button& btn);

private:
    void renderDayDuration(int dayDuration);
};
#endif
