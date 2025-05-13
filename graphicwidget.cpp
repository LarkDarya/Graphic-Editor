#include "graphicwidget.h"

GraphicWidget::GraphicWidget(QWidget *parent)
    : QWidget(parent)
{
    // Создаем QCustomPlot и настройваем layout
    m_plot = new QCustomPlot(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_plot);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Устанавливаем начальный диапазон для осей
    m_plot->xAxis->setRange(-10, 10);
    m_plot->yAxis->setRange(-10, 10);

    // Сетка
    m_plot->xAxis->grid()->setVisible(true);
    m_plot->yAxis->grid()->setVisible(true);

    connect(m_plot->xAxis, static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged),
            this, &GraphicWidget::onRangeChanged);
    connect(m_plot->yAxis, static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged),
            this, &GraphicWidget::onRangeChanged);
}

GraphicWidget::~GraphicWidget()
{
    clearFunctions();
}

int GraphicWidget::functionsCount() const
{
    return m_functions.size();
}

void GraphicWidget::addFunction(Function* func, const QColor& color)
{
    QCPGraph* graph = m_plot->addGraph();
    graph->setPen(QPen(color));

    QVector<double> xData, yData;
    const int pointsCount = 1000;
    double xMin = m_plot->xAxis->range().lower;
    double xMax = m_plot->xAxis->range().upper;

    // Для логарифмических функций корректируем диапазон
    if (func->getName() == "Logarithmic") {
        xMin = std::max(xMin, -10.0); // Минимальное значение для поиска асимптоты
    }

    double step = (xMax - xMin) / pointsCount;

    // Находим точку, где аргумент логарифма равен 0 (асимптота)
    double asymptoteX = std::numeric_limits<double>::max();
    if (func->getName() == "Logarithmic") {
        QVector<double> coeffs = func->getCoefficients();
        double c = coeffs[2];
        double d = coeffs[3];
        if (c != 0) {
            asymptoteX = -d/c;
        }
    }

    // Строим график
    for (int i = 0; i <= pointsCount; ++i) {
        double x = xMin + i * step;

        // Добавляем точки вблизи асимптоты для лучшего отображения
        if (func->getName() == "Logarithmic" &&
            std::abs(x - asymptoteX) < step*2) {
            // Точки слева от асимптоты
            double x_left = asymptoteX - step*10;
            double y_left = func->evaluate(x_left);
            xData.append(x_left);
            yData.append(y_left);

            // Точки справа от асимптоты
            double x_right = asymptoteX + step*10;
            double y_right = func->evaluate(x_right);
            xData.append(x_right);
            yData.append(y_right);
        }

        double y = func->evaluate(x);
        xData.append(x);
        yData.append(y);
    }

    // Настройки для отображения асимптоты
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPScatterStyle::ssNone);
    graph->setData(xData, yData);

    m_functions.append({func, graph});
    m_plot->replot();
}

void GraphicWidget::clearFunctions()
{
    for (auto& funcInfo : m_functions) {
        delete funcInfo.function;
        m_plot->removeGraph(funcInfo.graph);
    }
    m_functions.clear();
    m_plot->replot();
}

void GraphicWidget::setMainFunction(Function* func, const QColor& color)
{
    if (!m_functions.isEmpty())
    {
        // Заменяем данные первого графика
        FunctionInfo& mainInfo = m_functions[0];

        delete mainInfo.function;
        mainInfo.function = func;

        QVector<double> xData, yData;

        const int pointsCount = 1000;
        double xMin = m_plot->xAxis->range().lower;
        double xMax = m_plot->xAxis->range().upper;
        double step = (xMax - xMin) / pointsCount;

        for (int i = 0; i <= pointsCount; ++i)
        {
            double x = xMin + i * step;
            xData.append(x);
            yData.append(func->evaluate(x));
        }

        mainInfo.graph->setPen(QPen(color));
        mainInfo.graph->setData(xData, yData);
    }
    else
    {
        addFunction(func, color);
    }
    m_plot->replot();
}

void GraphicWidget::setSecondaryFunction(Function* func, const QColor& color)
{
    if (m_functions.size() > 1)
    {
        FunctionInfo& secondInfo = m_functions[1];

        delete secondInfo.function;
        secondInfo.function = func;

        QVector<double> xData, yData;

        const int pointsCount = 1000;
        double xMin = m_plot->xAxis->range().lower;
        double xMax = m_plot->xAxis->range().upper;
        double step = (xMax - xMin) / pointsCount;

        for (int i = 0; i <= pointsCount; ++i)
        {
            double x = xMin + i * step;
            xData.append(x);
            yData.append(func->evaluate(x));
        }

        secondInfo.graph->setPen(QPen(color));
        secondInfo.graph->setData(xData, yData);
    }
    else if (m_functions.size() == 1)
    {
        addFunction(func, color);
    }
    else
    {
        addFunction(func, color);
    }
    m_plot->replot();
}

void GraphicWidget::setXRange(double xmin, double xmax)
{
    m_plot->xAxis->setRange(xmin, xmax);
    updateAllFunctions();
}

void GraphicWidget::setYRange(double ymin, double ymax)
{
    m_plot->yAxis->setRange(ymin, ymax);
    updateAllFunctions();
}

void GraphicWidget::setRange(double xmin, double xmax, double ymin, double ymax)
{
    setXRange(xmin, xmax);
    setYRange(ymin, ymax);
}

void GraphicWidget::onRangeChanged(const QCPRange &newRange)
{
    Q_UNUSED(newRange);

    // Для каждого графика пересчитать данные по новому диапазону оси x
    const int pointsCount = 1000;
    double xMin = m_plot->xAxis->range().lower;
    double xMax = m_plot->xAxis->range().upper;
    double step = (xMax - xMin) / pointsCount;

    for (auto& funcInfo : m_functions)
    {
        QVector<double> xData, yData;
        for (int i = 0; i <= pointsCount; ++i)
        {
            double x = xMin + i * step;
            xData.append(x);
            yData.append(funcInfo.function->evaluate(x));
        }
        funcInfo.graph->setData(xData, yData);
    }

    m_plot->replot();
}

void GraphicWidget::updateAllFunctions()
{
    // При изменении диапазона обновляем все графики
    for (auto& funcInfo : m_functions)
    {
        QVector<double> xData, yData;

        const int pointsCount = 1000;
        double xMin = m_plot->xAxis->range().lower;
        double xMax = m_plot->xAxis->range().upper;
        double step = (xMax - xMin) / pointsCount;

        for (int i = 0; i <= pointsCount; ++i)
        {
            double x = xMin + i * step;
            xData.append(x);
            yData.append(funcInfo.function->evaluate(x));
        }

        funcInfo.graph->setData(xData, yData);
    }
    m_plot->replot();
}
