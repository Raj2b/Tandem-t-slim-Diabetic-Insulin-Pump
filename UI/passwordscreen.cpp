#include "passwordscreen.h"
#include "ui_passwordscreen.h"
#include <QString>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QWidget>
#include <QShowEvent>

passwordscreen::passwordscreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::passwordscreen)
{
    ui->setupUi(this);
    connectNumPad();
    // Confirm check
    //connect(ui->lockScreenCheck, &QPushButton::clicked, this, &passwordscreen::checkEnteredPin);

}

passwordscreen::~passwordscreen()
{
    delete ui;
}

void passwordscreen::handleDigitInput(const QString& digit) {
    if (enteredPin.length() < 6) {
        enteredPin += digit;
        ui->pinDisplayBox->setPlainText(enteredPin);

    }
}


void passwordscreen::checkEnteredPin() {
    QFile file("pin.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No PIN is set.");
        return;
    }

    QTextStream in(&file);
    QString savedPin = in.readLine().trimmed();

    if (enteredPin == savedPin) {
        emit successfulAuth();  // Allow access
    } else {
        QMessageBox::warning(this, "Incorrect PIN", "PINs do not match. Try again.");
        enteredPin.clear();
        ui->pinDisplayBox->clear();
    }
}

void passwordscreen::connectNumPad()
{
    connect(ui->pinButton0, &QPushButton::clicked, this, [this]() { handleDigitInput("0"); });
    connect(ui->pinButton1, &QPushButton::clicked, this, [this]() { handleDigitInput("1"); });
    connect(ui->pinButton2, &QPushButton::clicked, this, [this]() { handleDigitInput("2"); });
    connect(ui->pinButton3, &QPushButton::clicked, this, [this]() { handleDigitInput("3"); });
    connect(ui->pinButton4, &QPushButton::clicked, this, [this]() { handleDigitInput("4"); });
    connect(ui->pinButton5, &QPushButton::clicked, this, [this]() { handleDigitInput("5"); });
    connect(ui->pinButton6, &QPushButton::clicked, this, [this]() { handleDigitInput("6"); });
    connect(ui->pinButton7, &QPushButton::clicked, this, [this]() { handleDigitInput("7"); });
    connect(ui->pinButton8, &QPushButton::clicked, this, [this]() { handleDigitInput("8"); });
    connect(ui->pinButton9, &QPushButton::clicked, this, [this]() { handleDigitInput("9"); });

    connect(ui->pinBackButton, &QPushButton::clicked, this, [this]() {
        if (!enteredPin.isEmpty()) enteredPin.chop(1);
        updateDisplay();
    });

    connect(ui->confirmPinCheckBox, &QPushButton::clicked, this, &passwordscreen::checkEnteredPin);
}

void passwordscreen::updateDisplay() {
    if (ui->pinDisplayBox) {
        ui->pinDisplayBox->setText(enteredPin);
    }
}

void passwordscreen::resetPinInput() {
    enteredPin.clear();
    if (ui->pinDisplayBox) {
        ui->pinDisplayBox->clear();
    }
}

void passwordscreen::showEvent(QShowEvent* event) {
    QWidget::showEvent(event); // Don't forget base class!
    resetPinInput();
}

