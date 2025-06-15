#ifndef PINMANAGER_H
#define PINMANAGER_H

#include <QString>

class PinManager {
public:
    PinManager(const QString& filepath = "pin.txt");

    bool isPinSet() const;
    bool saveNewPin(const QString& pin);
    bool validatePin(const QString& pin);
    void removePin();
    QString getStoredPin() const;

private:
    QString pinFilePath;
};

#endif // PINMANAGER_H
