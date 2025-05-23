#ifndef RANGECONTROLLER_H
#define RANGECONTROLLER_H

#include <QObject>

class RangeController : public QObject {
    Q_OBJECT
signals:
    void rangeChanged(double xMin, double xMax, double yMin, double yMax);

public slots:
    void setRange(double xMin, double xMax, double yMin, double yMax);

public:
    explicit RangeController(QObject *parent = nullptr);
};

#endif // RANGECONTROLLER_H
