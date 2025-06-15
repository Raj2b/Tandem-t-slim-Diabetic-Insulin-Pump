#ifndef DATASTORAGESCREEN_H
#define DATASTORAGESCREEN_H

#include <QWidget>
#include "Modules/SessionRecorder.h"
#include "Modules/FileLogger.h"
#include "Controller.h"
#include "Session.h"
#include <QPushButton>
#include <vector>
#include <QString>
#include <QVBoxLayout>

namespace Ui {
class datastoragescreen;
}

class datastoragescreen : public QWidget
{
    Q_OBJECT

public:
    explicit datastoragescreen(SessionRecorder& session, Controller& controller, QWidget *parent = nullptr);
    ~datastoragescreen();

private:
    Ui::datastoragescreen *ui;

    SessionRecorder& currSession;
    Controller& currController;
    FileLogger& currLogger;
    QVBoxLayout* layout;
    QWidget* container;

    int desiredLogs = 0;
    int currentPage = 1; //1 indicated Home
    std::vector<QPushButton*> foundSessions;

    void reset();
    void retrieveSysLogs(int);
    void retrieveHealthLogs(int);
    void retrieveSessionValues(QString);
    void defaultState();
    void pageOperation(int, int); //Desired Logs, and Direction


};

#endif // DATASTORAGESCREEN_H
