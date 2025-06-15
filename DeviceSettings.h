#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#include <QDebug>
#include "Profile.h"

class DeviceSettings : public QObject {
    Q_OBJECT
public:
    Profile getProfile() {
        return pfl;
    }

    void setProfile(Profile& profile) {
        pfl = profile;
        emit profileChanged();
    }

signals:
    void profileChanged();

private:
    Profile pfl;
};

#endif // DEVICESETTINGS_H
