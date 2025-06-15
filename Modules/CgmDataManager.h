#ifndef CGMDATAMANAGER_H
#define CGMDATAMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>

struct GlucosePoint {
    QDateTime time;
    float value;
};

class CgmDataManager : public QObject {
    Q_OBJECT

public:
    explicit CgmDataManager(QObject* parent = nullptr);
    const QVector<GlucosePoint>& getHistory() const;

signals:
    void newCgmPoint(const GlucosePoint& point);

private slots:
    void generatePoint();

private:
    QTimer timer;
    QVector<GlucosePoint> history;
    float lastValue;
};

#endif // CGMDATAMANAGER_H
