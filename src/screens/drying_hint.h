#ifndef DRYINGHINTSCREEN_H
#define DRYINGHINTSCREEN_H

#include "../screen.h"

class DryingHintScreen : public Screen {
public:
    DryingHintScreen();
    String getName() { return "dryinghintScreen"; }
    void render();
    void renderButton(Button& btn);
    void renderButtonPressed(Button& btn);
    void handleButton(Button& btn);
    void update();
};
#endif
