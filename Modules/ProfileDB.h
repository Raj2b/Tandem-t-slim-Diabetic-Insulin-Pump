#ifndef PROFILEDB_H
#define PROFILEDB_H

#include <QTextStream>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QString>
#include <QTime>
#include <QJsonArray>
#include <QFile>
#include <QDir>

#include <string>
#include <vector>

#include "Profile.h"

class ProfileDB {
public:
    ProfileDB(QString path, QString fileExtension);

    void createProfile(Profile& profile);
    Profile readProfile(QString profileName);
    void updateProfile(Profile& profile);
    void deleteProfile(QString profileName);
    QStringList getProfileTimes(QString profileName);
    QStringList fetchProfiles();

private:
    QDir qdir;
    const QString profilesPath;
    const QString fileExt;
};


#endif // PROFILEDB_H
