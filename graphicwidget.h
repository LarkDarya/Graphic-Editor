#ifndef GRAPHICWIDGET_H
#define GRAPHICWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include "Function.h"

class GraphicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphicWidget(QWidget *parent = nullptr);
    ~GraphicWidget();

    int functionsCount() const;
    void addFunction(Function* func, const QColor& color = QColor("#1E2A78"));
    void clearFunctions();
    void setMainFunction(Function* func, const QColor& color = QColor("#1E2A78"));
    void setSecondaryFunction(Function* func, const QColor& color = QColor("#FF2E4C"));
    void setXRange(double xmin, double xmax);
    void setYRange(double ymin, double ymax);
    void setRange(double xmin, double xmax, double ymin, double ymax);

private:
    struct FunctionInfo {
        Function* function;
        QCPGraph* graph;
    };
    QVector<FunctionInfo> m_functions;
    QCustomPlot* m_plot;

    void onRangeChanged(const QCPRange &newRange);
    void updateAllFunctions();
};

#endif // GRAPHICWIDGET_H
