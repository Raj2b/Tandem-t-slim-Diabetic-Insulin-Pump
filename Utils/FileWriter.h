#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <stdexcept>

namespace FileWriter {

inline bool fileExists(QString dirPath, QString filename) {
    QDir qdir(dirPath);
    QFile file( qdir.filePath(filename) );
    return file.exists();
}

inline void createFile(QString dirPath, QString filename) {
    QDir qdir(dirPath);
    QFile file( qdir.filePath(filename) );

    if( !file.open(QIODevice::WriteOnly) ) {
        throw std::runtime_error("Could not create file: " + filename.toStdString());
    }
    file.close();
}

inline QString readFile(QString dirPath, QString filename) {
    QDir qdir(dirPath);
    QFile file( qdir.filePath(filename) );

    if (!file.exists()) {
        throw std::runtime_error("File not found: " + filename.toStdString());
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Could not open file: " + filename.toStdString());
    }

    QByteArray fileContents = file.readAll();
    file.close();
    return QString(fileContents);
}

inline void overwrite(QString dirPath, QString filename, QString contents) {
    QDir qdir(dirPath);
    QFile file( qdir.filePath(filename) );

    if (!file.exists()) {
        throw std::runtime_error("File not found: " + filename.toStdString());
    }

    if( !file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) ) {
        throw std::runtime_error("Could not open file: " + filename.toStdString());
    }

    if( file.write(contents.toUtf8()) == -1 ) {
        throw std::runtime_error("Could not write to file: " + filename.toStdString());
    }
}

inline void appendFile(QString dirPath, QString filename, QString contents) {
    QDir qdir(dirPath);
    QFile file( qdir.filePath(filename) );

    if (!file.exists()) {
        throw std::runtime_error("File not found: " + filename.toStdString());
    }

    if( !file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append) ) {
        throw std::runtime_error("Could not open file: " + filename.toStdString());
    }

    if( file.write(contents.toUtf8()) == -1 ) {
        throw std::runtime_error("Could not write to file: " + filename.toStdString());
    }
}

inline void removeFile(QString dirPath, QString filename) {
    QDir qdir(dirPath);
    QFile file( qdir.filePath(filename) );

    if (file.exists()) {
        if ( !file.remove() ) {
            throw std::runtime_error("Failed to delete: " + filename.toStdString());
        }
    } else {
        throw std::runtime_error("File not found: " + filename.toStdString());
    }
}

};

#endif // FILEWRITER_H
