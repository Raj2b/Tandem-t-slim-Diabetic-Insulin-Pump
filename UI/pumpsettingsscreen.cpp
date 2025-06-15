#include "pumpsettingsscreen.h"
#include "ui_pumpsettingsscreen.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>

pumpsettingsscreen::pumpsettingsscreen(BasalPump& basalpump, /*DeviceSettings& deviceSettings, */QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pumpsettingsscreen),
    bp(basalpump)/*,
    ds(deviceSettings)*/
{
    ui->setupUi(this);
    connectNumPad();
    pinManager = new PinManager();
    ui->goToSetPinButton->setVisible(false);
    ui->securityPinButton->setStyleSheet(R"(
    QPushButton {
        background-color: #444;
        color: white;
        border: 2px solid #888;
        border-radius: 5px;
    }
    QPushButton:checked {
        background-color: #00aa00;
        color: white;
        border: 2px solid #00cc00;
    }
)");

    menuButtons = {
        ui->stopInsulinButton,
        ui->myPumpButton,
        ui->tempRateButton,
        ui->loadButton,
        ui->logsButton,
        ui->deviceSettings
    };

    updateVisibleButtons();

    connect(ui->downButton, &QPushButton::clicked, this, [this]() { if (currentMenuPage < (menuButtons.size() - 1) / 4) { currentMenuPage++; updateVisibleButtons(); } });
    connect(ui->upButton, &QPushButton::clicked, this, [this]() { if (currentMenuPage > 0) { currentMenuPage--; updateVisibleButtons(); } });
    connect(ui->deviceSettings, &QPushButton::clicked, this, [this]() { ui->stackedWidget->setCurrentWidget(ui->deviceSettingsPage); });
    connect(ui->homeButton, &QPushButton::clicked, this, [this]() { emit clickHome(); });
    connect(ui->homeButton_2, &QPushButton::clicked, this, [this]() { ui->stackedWidget->setCurrentIndex(0); currentMenuPage = 0; updateVisibleButtons(); });
    connect(ui->page9CancelButton, &QPushButton::clicked, this, &pumpsettingsscreen::handleCancelPinScreen);
    connect(ui->securityPinButton, &QPushButton::clicked, this, &pumpsettingsscreen::handleSecurityPinToggled);
    connect(ui->goToSetPinButton, &QPushButton::clicked, this, &pumpsettingsscreen::handleGoToSetPinClicked);

    connect(ui->stopInsulinButton, &QPushButton::clicked, [this]() { toggleInsulin(); });
    connect(ui->logsButton, &QPushButton::clicked, this, [this]() {emit switchToLogs();});
}

void pumpsettingsscreen::toggleInsulin() {
    bp.isRunning() ? bp.stop() : bp.resume();
}

QPushButton* pumpsettingsscreen::getMyPumpButton() {return ui->myPumpButton;}


pumpsettingsscreen::~pumpsettingsscreen() {
    delete pinManager;
    delete ui;
}


void pumpsettingsscreen::showMainMenuPage() {ui->stackedWidget->setCurrentIndex(0);currentMenuPage = 0;updateVisibleButtons();}
void pumpsettingsscreen::updateVisibleButtons() { QLayoutItem *item; while ((item = ui->verticalLayout->takeAt(0)) != nullptr) { if (item->widget()) { item->widget()->hide(); ui->verticalLayout->removeWidget(item->widget()); } delete item; } int startIndex = currentMenuPage * 4; int endIndex = std::min(startIndex + 4, static_cast<int>(menuButtons.size())); for (int i = startIndex; i < endIndex; ++i) { ui->verticalLayout->addWidget(menuButtons[i]); menuButtons[i]->show(); } }
void pumpsettingsscreen::handleSecurityPinToggled() {
    newPinInput.clear();
    confirmPinInput.clear();
    pinBuffer.clear();
    if (ui->securityPinButton->isChecked()) {
        newPinInput.clear();
        confirmPinInput.clear();
        ui->securityPinButton->setText("Security PIN: ON");
        ui->goToSetPinButton->setVisible(true);
    } else {
        pinManager->removePin();
        newPinInput.clear();
        confirmPinInput.clear();
        QMessageBox::information(this, "PIN Removed", "Security PIN has been disabled.");
        ui->securityPinButton->setText("Security PIN: OFF");
        ui->goToSetPinButton->setVisible(false);
    }
}
void pumpsettingsscreen::handleNewPinInput(const QString& digit) { if (newPinInput.isEmpty() && digit == "0") return; if (newPinInput.length() >= 6) return; newPinInput += digit; }

void pumpsettingsscreen::handleConfirmPinInput(const QString& digit) {if (confirmPinInput.length() >= newPinInput.length()) return;confirmPinInput += digit;}
void pumpsettingsscreen::on_confirmPinScreen_customContextMenuRequested(const QPoint &pos){Q_UNUSED(pos);}
void pumpsettingsscreen::handleNewPinCheckClicked() { int length = newPinInput.length(); if (length >= 4 && length <= 6) { ui->stackedWidget->setCurrentWidget(ui->confirmNewPinScreen); } else { QMessageBox::warning(this, "Invalid PIN", "Please enter a PIN between 4 and 6 digits, then press ✓."); newPinInput.clear(); } }

void pumpsettingsscreen::handleConfirmPinCheckClicked() {
    if (confirmPinInput == newPinInput && !confirmPinInput.isEmpty()) {
        pinManager->saveNewPin(newPinInput);
        QMessageBox::information(this, "PIN Set", "Your Security PIN has been created.");
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        confirmPinInput.clear();
        QMessageBox::warning(this, "Mismatch", "PINs do not match. Please try again.");
    }
}

void pumpsettingsscreen::connectNumPad()
{
    connect(ui->pinButton0, &QPushButton::clicked, this, [this]() { handlePinDigit("0"); });
    connect(ui->pinButton1, &QPushButton::clicked, this, [this]() { handlePinDigit("1"); });
    connect(ui->pinButton2, &QPushButton::clicked, this, [this]() { handlePinDigit("2"); });
    connect(ui->pinButton3, &QPushButton::clicked, this, [this]() { handlePinDigit("3"); });
    connect(ui->pinButton4, &QPushButton::clicked, this, [this]() { handlePinDigit("4"); });
    connect(ui->pinButton5, &QPushButton::clicked, this, [this]() { handlePinDigit("5"); });
    connect(ui->pinButton6, &QPushButton::clicked, this, [this]() { handlePinDigit("6"); });
    connect(ui->pinButton7, &QPushButton::clicked, this, [this]() { handlePinDigit("7"); });
    connect(ui->pinButton8, &QPushButton::clicked, this, [this]() { handlePinDigit("8"); });
    connect(ui->pinButton9, &QPushButton::clicked, this, [this]() { handlePinDigit("9"); });

    connect(ui->pinBackButton, &QPushButton::clicked, this, [this]() {
        if (!pinBuffer.isEmpty()) pinBuffer.chop(1);
        updatePinDisplay();
    });

    connect(ui->confirmPinCheckBox, &QPushButton::clicked, this, [this]() {
        finalizePinEntry();
    });
}

void pumpsettingsscreen::updatePinDisplay() {
    if (ui->page9TextBox) {
        ui->page9TextBox->setPlainText(pinBuffer);
    }
}

void pumpsettingsscreen::handlePinDigit(const QString& digit)
{
    if (pinBuffer.length() < 6) {
        if (pinBuffer.isEmpty() && digit == "0") return;  // Don't allow starting with 0
        pinBuffer += digit;
        updatePinDisplay();
    }
}

void pumpsettingsscreen::handleGoToSetPinClicked() {
    newPinInput.clear();
    confirmPinInput.clear();  // << add this
    pinBuffer.clear();
    pinMode = PinEntryMode::NewPin;
    updatePinDisplay();
    ui->page9MainLabel->setText("Enter PIN");  // update label
    ui->stackedWidget->setCurrentWidget(ui->changeExistingPasswordScreen);
}


void pumpsettingsscreen::goToConfirmPin() {
    if (newPinInput.isEmpty()) {
        QMessageBox::warning(this, "Invalid", "PIN cannot be empty.");
        return;
    }

    confirmPinInput.clear();  // ← ADD THIS

    pinMode = PinEntryMode::ConfirmPin;
    pinBuffer.clear();
    updatePinDisplay();
    ui->page9MainLabel->setText("Re-enter PIN");
    ui->confirmPinCheckBox->setStyleSheet(R"(
        QPushButton {
            font: 75 35pt "DejaVu Sans";
            background-color: rgb(0, 170, 0); /* Green */
            color: white;
        }
    )");
    ui->stackedWidget->setCurrentWidget(ui->changeExistingPasswordScreen);
}



void pumpsettingsscreen::finalizePinEntry()
{
    qDebug() << "[DEBUG] Finalizing PIN Entry";
    qDebug() << "Current pinMode:" << static_cast<int>(pinMode);
    qDebug() << "pinBuffer:" << pinBuffer;

    if (pinBuffer.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "PIN cannot be empty.");
        return;
    }

    if (pinMode == PinEntryMode::NewPin) {
        if (pinBuffer.length() < 4) {
            QMessageBox::warning(this, "Too Short", "PIN must be at least 4 digits.");
            return;
        }
        newPinInput = pinBuffer;
        qDebug() << "[DEBUG] Saved newPinInput:" << newPinInput;
        goToConfirmPin();  // <- This clears the pinBuffer

    } else if (pinMode == PinEntryMode::ConfirmPin) {
        confirmPinInput = pinBuffer;
        qDebug() << "[DEBUG] Comparing: confirmPinInput:" << confirmPinInput << " vs newPinInput:" << newPinInput;

        if (confirmPinInput == newPinInput) {
            pinManager->saveNewPin(newPinInput);
            QMessageBox::information(this, "PIN Set", "Your Security PIN has been created.");
            ui->stackedWidget->setCurrentIndex(0); // back to main
        } else {
            QMessageBox::warning(this, "Mismatch", "PINs do not match. Try again.");
            confirmPinInput.clear(); // ← ADD THIS to reset stale confirm input
            goToConfirmPin();  // retry
        }

    }

    pinBuffer.clear();
}


void pumpsettingsscreen::handleCancelPinScreen() {
    pinBuffer.clear();
    updatePinDisplay();

    if (pinMode == PinEntryMode::ConfirmPin) {
        pinMode = PinEntryMode::NewPin;
        ui->page9MainLabel->setText("Enter PIN");
        ui->stackedWidget->setCurrentWidget(ui->changeExistingPasswordScreen);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->deviceSettingsPage);
    }
}
