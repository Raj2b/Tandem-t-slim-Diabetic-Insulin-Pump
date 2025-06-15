#include "FileLogger.h"

FileLogger::FileLogger(QString path, QString filename)
    : /*EventLogger(),*/
      qdir(QCoreApplication::applicationDirPath()),
      fn(filename)
{
    if (!qdir.exists(path)) {
        qdir.mkdir(path);
    }
    qdir.cd(path);

    if(!FileWriter::fileExists(qdir.path(), fn)) {
        FileWriter::createFile(qdir.path(), fn);
    }
}

void FileLogger::log(QString msg) {
    try {
        QString lineHeader = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "] ";
        FileWriter::appendFile(qdir.path(), fn, lineHeader + msg + "\n");
    } catch (std::exception& e) {
        qDebug() << "Could not log message | " << e.what();
    }


//    QString filename = "Log" + fileExt;
//    QFile file( qdir.filePath(filename) );

//    if (file.open(QIODevice::Append | QIODevice::Text)) {
//        QTextStream out(&file);
//        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
//            << "] "
//            << msg;

//        file.close();
//        qDebug() << "Message Logged";

//    } else {
//        qDebug() << "Unable to open log";

//    }
}

QString FileLogger::fetch() {
    return FileWriter::readFile(qdir.currentPath() + "/Logs/", fn);
}

QStringList FileLogger::fetchLinesEndingAt(int lineNumber) {
    QString fullContent = fetch();
    QStringList lines = fullContent.split('\n', Qt::SkipEmptyParts);

    int end = qBound(0, lineNumber, lines.size());   //Ensure not past end of list
    int start = qMax(0, end - 5);                    //Start must be 0

    return lines.mid(start, end - start); //Return up to 5 lines ending at 'lineNumber'
}
