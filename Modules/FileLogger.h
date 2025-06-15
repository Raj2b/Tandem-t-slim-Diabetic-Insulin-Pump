#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <string>

#include "Utils/FileWriter.h"

class FileLogger : public QObject {
    Q_OBJECT
public:
    FileLogger(QString path, QString fileExtension);
public slots:
    void log(QString msg);
    QString fetch();
    QStringList fetchLinesEndingAt(int);

private:
    QDir qdir;
    const QString fn;
};

#endif // FILELOGGER_H
