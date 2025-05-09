#ifndef GRAPHICWIDGET_H
#define GRAPHICWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QVector>
#include <QPen>
#include <QDebug>
#include <cmath>
#include "Function.h"

class GraphicWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicWidget(QWidget *parent = nullptr)
        : QWidget(parent), m_xMin(-10), m_xMax(10), m_yMin(-10), m_yMax(10)
    {
        setMinimumSize(600, 400);
        setAutoFillBackground(true);
        m_background = QPixmap(size());
    }

    // Добавление функции для отображения
    void addFunction(Function* func, const QColor& color = Qt::blue)
    {
        m_functions.append({func, color});
        updateBackground();
        update();
    }

    // Очистка всех функций
    void clearFunctions()
    {
        for (auto& funcInfo : m_functions) {
            delete funcInfo.function;
        }
        m_functions.clear();
        update();
    }

    void setMainFunction(Function* func, const QColor& color) {
        if (m_functions.size() > 0) {
            delete m_functions[0].function;
            m_functions[0] = {func, color};
        } else {
            m_functions.append({func, color});
        }
        update();
    }

    void setSecondaryFunction(Function* func, const QColor& color) {
        if (m_functions.size() > 1) {
            delete m_functions[1].function;
            m_functions[1] = {func, color};
        } else if (m_functions.size() == 1) {
            m_functions.append({func, color});
        } else {
            // Если нет основного графика, добавляем как основной
            m_functions.append({func, color});
        }
        update();
    }

    // Установка диапазонов
    void setXRange(double xmin, double xmax)
    {
        m_xMin = xmin;
        m_xMax = xmax;
        updateBackground();
        update();
    }

    void setYRange(double ymin, double ymax)
    {
        m_yMin = ymin;
        m_yMax = ymax;
        updateBackground();
        update();
    }

    void setRange(double xmin, double xmax, double ymin, double ymax)
    {
        setXRange(xmin, xmax);
        setYRange(ymin, ymax);
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);

        // Рисуем сохраненный фон
        painter.drawPixmap(0, 0, m_background);

        // Рисуем текущие функции поверх
        drawFunctions(painter);
    }

    void resizeEvent(QResizeEvent* event) override
    {
        QWidget::resizeEvent(event);
        m_background = QPixmap(size());
        updateBackground();
    }

private:
    struct FunctionInfo {
        Function* function;
        QColor color;
    };

    QVector<FunctionInfo> m_functions;
    double m_xMin, m_xMax;
    double m_yMin, m_yMax;
    const int margin = 50; // увеличенные отступы
    QPixmap m_background;


    // Преобразование координат
    int mapX(double x) const
    {
        double w = width() - 2 * margin;
        return margin + static_cast<int>((x - m_xMin) / (m_xMax - m_xMin) * w);
    }

    int mapY(double y) const
    {
        double h = height() - 2 * margin;
        return margin + static_cast<int>((m_yMax - y) / (m_yMax - m_yMin) * h);
    }

    // Обновление фона (осей и сетки)
    void updateBackground()
    {
        m_background.fill(Qt::white);
        QPainter painter(&m_background);
        painter.setRenderHint(QPainter::Antialiasing);

        drawGrid(painter);
        drawAxes(painter);

        if (m_functions.isEmpty()) {
            painter.setPen(Qt::black);
            QFont font("Cambria", 12);
            painter.setFont(font);
            painter.drawText(rect(), Qt::AlignCenter, "Функция не задана");
        }
    }

    // Рисование функций
    void drawFunctions(QPainter& painter)
    {
        painter.setRenderHint(QPainter::Antialiasing);

        for (const auto& funcInfo : m_functions) {
            drawFunction(painter, funcInfo.function, funcInfo.color);
        }
    }

    // Рисование одной функции
    void drawFunction(QPainter& painter, Function* function, const QColor& color)
    {
        int w = width() - 2 * margin;
        if (w <= 0) return;

        QPainterPath path;
        bool firstPoint = true;

        // Увеличиваем количество точек для более плавного графика
        int pointCount = w * 2;
        for (int i = 0; i <= pointCount; ++i) {
            double x = m_xMin + (m_xMax - m_xMin) * i / pointCount;
            double y = function->evaluate(x);

            if (y >= m_yMin && y <= m_yMax) {
                QPoint p(mapX(x), mapY(y));
                if (firstPoint) {
                    path.moveTo(p);
                    firstPoint = false;
                } else {
                    path.lineTo(p);
                }
            } else {
                firstPoint = true;
            }
        }

        painter.setPen(QPen(color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(path);
    }

    // Рисование осей
    void drawAxes(QPainter& painter)
    {
        painter.setPen(QPen(Qt::black, 2));

        // Ось X
        int y0 = mapY(0);
        painter.drawLine(margin, y0, width() - margin, y0);

        // Ось Y
        int x0 = mapX(0);
        painter.drawLine(x0, margin, x0, height() - margin);

        // Подписи осей
        painter.setPen(Qt::black);
        QFont font("Cambria", 10);
        painter.setFont(font);

        // Отметки по X
        int n = 10;
        for (int i = 0; i <= n; ++i) {
            double xVal = m_xMin + i * (m_xMax - m_xMin) / n;
            int xPos = mapX(xVal);
            painter.drawLine(xPos, y0 - 5, xPos, y0 + 5);
            painter.drawText(xPos - 15, y0 + 20, QString::number(xVal, 'f', 1));
        }

        // Отметки по Y
        for (int i = 0; i <= n; ++i) {
            double yVal = m_yMin + i * (m_yMax - m_yMin) / n;
            int yPos = mapY(yVal);
            painter.drawLine(x0 - 5, yPos, x0 + 5, yPos);
            painter.drawText(x0 - 35, yPos + 5, QString::number(yVal, 'f', 1));
        }
    }

    // Рисование сетки
    void drawGrid(QPainter& painter)
    {
        painter.setPen(QPen(QColor(220, 220, 220), 1));

        int n = 10;
        // Вертикальные линии
        for (int i = 0; i <= n; ++i) {
            double xVal = m_xMin + i * (m_xMax - m_xMin) / n;
            int xPos = mapX(xVal);
            painter.drawLine(xPos, margin, xPos, height() - margin);
        }

        // Горизонтальные линии
        for (int i = 0; i <= n; ++i) {
            double yVal = m_yMin + i * (m_yMax - m_yMin) / n;
            int yPos = mapY(yVal);
            painter.drawLine(margin, yPos, width() - margin, yPos);
        }
    }
};

#endif // GRAPHICWIDGET_H
