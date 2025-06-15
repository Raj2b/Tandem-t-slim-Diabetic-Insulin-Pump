#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QMainWindow>
#include <stack>
#include <memory>

#include "Controller.h"

//#include all UIs

class WindowManager : public QObject {
    Q_OBJECT

public:
    WindowManager(Controller& ctlr);

    void pushUI(std::shared_ptr<QMainWindow> ui);
    void popUI();

private:
    void linkSignals();

    Controller& c;

    std::stack< std::shared_ptr<QMainWindow> > displays;
    //module UIs here, all should be sharedptrs

};
#endif // WINDOWMANAGER_H
