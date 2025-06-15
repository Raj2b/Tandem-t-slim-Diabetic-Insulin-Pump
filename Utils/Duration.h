#ifndef DURATION_H
#define DURATION_H

#include <QString>

struct Duration {
    qint64 d;
    qint64 h;
    qint64 m;
    qint64 s;

    QString toString() {
        return  QString::number(d)+"d "
                + QString::number(h)+"h "
                + QString::number(m)+"m "
                + QString::number(s)+"s";
    }

    qint64 toSecs() {
        return d*24*60*60 + h*60*60 + m*60 + s*60;
    }
};

#endif // DURATION_H
