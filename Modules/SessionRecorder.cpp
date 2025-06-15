#include "SessionRecorder.h"
#include <QDebug>

SessionRecorder::SessionRecorder(QString path, QString fileExtension)
    : qdir(QCoreApplication::applicationDirPath()),
      fileExt(fileExtension)
{
    if (!qdir.exists(path)) {
        qdir.mkdir(path);
    }
    qdir.cd(path);
}

QStringList SessionRecorder::fetchSessions() {
    QStringList filenames;
    filenames = qdir.entryList(QStringList() << "*." + fileExt, QDir::Files);
    return filenames;
}

void SessionRecorder::record(Session& session) {
    try {
        QString filename = session.getStartDT().toString("yyyy-MM-dd (HH-mm-ss)") + fileExt;

        FileWriter::createFile(qdir.path(), filename);
        FileWriter::overwrite(
                    qdir.path(),
                    filename,
                    QJsonDocument( session.toJSON()).toJson(QJsonDocument::Indented) );
    } catch (std::exception& e) {
        qDebug() << "Session could not be recorded | " << e.what();
    }
}

Session SessionRecorder::recoverSession(QString sessionName) {
    QString json = FileWriter::readFile(qdir.path(), sessionName + fileExt);
    try {
        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        if(!doc.isObject()) {
            throw std::runtime_error("JSON invalid");
        }
        return Session::fromJSON( doc.object() );

    } catch (std::exception& e) {
        qDebug() << "Failed to parse file: " << sessionName << " | " << e.what();
        return Session();
    }
}

QStringList SessionRecorder::fetchSessionsNew() {
    QStringList filenames;
    QDir temp(QCoreApplication::applicationDirPath() + "/Sessions");

    QStringList allFiles = temp.entryList(QDir::Files);  // Get full filenames
    for (const QString& file : allFiles) {
        QFileInfo info(file);
        filenames.append(info.completeBaseName());  // Removes only the last extension
    }
    //qDebug() << filenames;
    return filenames;
}
