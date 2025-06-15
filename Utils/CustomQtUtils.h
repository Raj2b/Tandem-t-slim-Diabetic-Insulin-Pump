#ifndef CUSTOMQTUTILS_H
#define CUSTOMQTUTILS_H

#include <QDateTime>

namespace CustomQtUtils {

inline QTime qtimeTruncate(const QTime& t) {
    return QTime(t.hour(), t.minute(), t.second());
}

inline QDateTime currentMajorDateTime() {
    return QDateTime(QDate::currentDate(), qtimeTruncate(QTime::currentTime()));
}

}

#endif // CUSTOMQTUTILS_H
