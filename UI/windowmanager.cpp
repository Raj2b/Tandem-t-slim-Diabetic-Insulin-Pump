#include "windowmanager.h"

WindowManager::WindowManager(Controller& ctlr)
    : c(ctlr)
{
    linkSignals();
}

void WindowManager::linkSignals() {
    connect(&c, &Controller::stateOFF, [this]() {
        //pop all UIs
    });

    connect(&c, &Controller::stateIDLE, [this]() {
        //push HOME UI
    });

}

//---------------------------------------------
void WindowManager::popUI() {
    if (!displays.empty()) {
        displays.top()->close();
        displays.pop();
    }
}

void WindowManager::pushUI(std::shared_ptr<QMainWindow> ui)  {
    if (!displays.empty()) {
        displays.top()->close();
    }

    displays.push(ui);
    displays.top()->show();
}
