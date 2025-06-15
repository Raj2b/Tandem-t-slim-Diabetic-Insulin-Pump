#include "PinManager.h"
#include <QFile>
#include <QTextStream>

PinManager::PinManager(const QString& filepath) : pinFilePath(filepath) {}

bool PinManager::isPinSet() const {
    QFile file(pinFilePath);
    return file.exists();
}

QString PinManager::getStoredPin() const {
    QFile file(pinFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return "";
    QTextStream in(&file);
    return in.readLine();
}

bool PinManager::saveNewPin(const QString& pin) {
    QFile file(pinFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out << pin;
    file.close();
    return true;
}

bool PinManager::validatePin(const QString& pin) {
    return pin == getStoredPin();
}

void PinManager::removePin() {
    QFile::remove(pinFilePath);
}
