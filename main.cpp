#include <QApplication>

#include "UI/mainwindow.h"
#include "Controller.h"
#include "Person.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Person user;
    Controller controller(user);

    //window manager takes a reference of controller
    //and distributes the neccessary modules to its UIs
    MainWindow w(controller);

    w.show();
    return a.exec();
}
