#ifndef SESSIONRECORDER_H
#define SESSIONRECORDER_H

#include <QCoreApplication>
#include <QDebug>
#include <QString>

#include "Session.h"
#include "Utils/FileWriter.h"

class SessionRecorder {
public:
    SessionRecorder(QString path, QString fileExtension);

    void record(Session& session);
    QStringList fetchSessions();
    QStringList fetchSessionsNew();
    Session recoverSession(QString sessionName);

private:
    QDir qdir;
    const QString fileExt;
};

#endif // SESSIONRECORDER_H
