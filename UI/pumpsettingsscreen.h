#ifndef PUMPSETTINGSSCREEN_H
#define PUMPSETTINGSSCREEN_H

#include <QWidget>
#include <QPushButton>

#include "PinManager.h"
#include "Controller.h"

namespace Ui {
class pumpsettingsscreen;
}

class pumpsettingsscreen : public QWidget
{
    Q_OBJECT

public:
    explicit pumpsettingsscreen(BasalPump& basalpump, /*DeviceSettings& deviceSettings, */QWidget *parent = nullptr);
    ~pumpsettingsscreen();
    QPushButton* getMyPumpButton();
    void showMainMenuPage();
    void handleNewPinInput(const QString& digit);

    void handleConfirmPinInput(const QString& digit);
    void handleGoToSetPinClicked();
    void connectNumPad();
    void updatePinDisplay();
    void handlePinDigit(const QString& digit);
    void goToConfirmPin();
    void finalizePinEntry();
    void handleCancelPinScreen();

signals:
    void clickHome();
    void switchToLogs();

private slots:
    //void onChangePinClicked();

    void handleSecurityPinToggled();
    void handleNewPinCheckClicked();
    void handleConfirmPinCheckClicked();  //Called when ✓ is clicked on confirm screen

    void on_confirmPinScreen_customContextMenuRequested(const QPoint &pos);

    void toggleInsulin();

private:
    Ui::pumpsettingsscreen *ui;
    QVector<QPushButton*> menuButtons;
    int currentMenuPage = 0; //0 = top half, 1 = bottom half

    void updateVisibleButtons();
    QString newPinInput;
    QString confirmPinInput;
    QString pinBuffer;
    enum class PinEntryMode { None, NewPin, ConfirmPin };
    PinEntryMode pinMode = PinEntryMode::None;

    PinManager* pinManager;

    BasalPump& bp;
//    DeviceSettings& ds;

};

#endif // PUMPSETTINGSSCREEN_H
