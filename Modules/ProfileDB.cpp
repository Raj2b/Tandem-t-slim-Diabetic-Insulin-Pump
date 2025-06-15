#include "ProfileDB.h"

ProfileDB::ProfileDB(QString path, QString fileExtension)
    : qdir(QCoreApplication::applicationDirPath()),
      profilesPath(path),
      fileExt(fileExtension)
{
    if (!qdir.exists(profilesPath)) {
        qdir.mkdir(profilesPath);
    }
    qdir.cd(profilesPath); //enter Profiles/*
}

QStringList ProfileDB::fetchProfiles(){
    QStringList profileNames;
    //filter: files with extension
    profileNames = qdir.entryList(QStringList() << "*" + fileExt, QDir::Files);
    return profileNames;
}

//--------------------------------

void ProfileDB::createProfile(Profile& profile){
    QString filename = profile.name + fileExt;
    QFile file( qdir.filePath(filename) );
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "Failed to create profile file: " << filename;
        return;
    }

    QJsonDocument doc( profile.toJSON() );

    file.write(doc.toJson());
    file.close();
}

Profile ProfileDB::readProfile(QString profileName) {
    QString filename = profileName + fileExt;
    QFile file( qdir.filePath(filename) );
    if (!file.exists()) {
         qDebug() << "File not found: " << filename;
         return Profile();
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
         qDebug() << "Failed to open file: " << filename;
         return Profile();
    }

    QByteArray fileContents = file.readAll();
    file.close();

    try {
        QJsonDocument doc = QJsonDocument::fromJson(fileContents);
        if(!doc.isObject()) {
            throw std::runtime_error("JSON invalid");
        }
        return Profile::fromJSON( doc.object() );

    } catch (std::exception& e) {
        qDebug() << "Failed to parse file: " << filename << " | " << e.what();
        return Profile();
    }
}

void ProfileDB::updateProfile(Profile &profile) {
    createProfile(profile);
}

void ProfileDB::deleteProfile(QString profileName){
    QString filename = profileName + fileExt;

    qDebug() << "[Debug] Deleting: " << filename;
    QFile file( qdir.filePath(filename) );
    if (file.exists()) {
        if ( file.remove() ) {
            qDebug() << "Deleted profile: " << filename;
        } else {
             qDebug() << "Failed to delete profile: " << filename;
        }
    } else {
         qDebug() << "Profile file does not exist: " << filename;
    }
}

QStringList ProfileDB::getProfileTimes(QString profileName){
    Profile toFind = readProfile(profileName);
    QStringList timeStrings;

    for (const auto& pair : toFind.timepoints) {
        QTime time = pair.first;                        //get the Qtime
        QString timeStr = time.toString("HH:mm:ss");    //convert to QString
        timeStrings << timeStr;                         //Add to QStringList
    }
    return timeStrings;
}
