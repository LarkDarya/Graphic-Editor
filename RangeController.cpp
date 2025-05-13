#include "RangeController.h"

RangeController::RangeController(QObject *parent)
    : QObject(parent)
{
}

void RangeController::setRange(double xMin, double xMax, double yMin, double yMax) {
    emit rangeChanged(xMin, xMax, yMin, yMax);
}
