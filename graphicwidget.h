#ifndef GRAPHICWIDGET_H
#define GRAPHICWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QVector>
#include <QPen>
#include <QDebug>
#include "Function.h"

class GraphicWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicWidget(QWidget *parent = nullptr)
        : QWidget(parent), m_function(nullptr),
        m_xMin(-10), m_xMax(10), m_yMin(-10), m_yMax(10)
    {
        setMinimumSize(400, 300);
    }

    // Установка функции для отображения
    void setFunction(Function* func)
    {
        m_function = func;
        update(); // перерисовать виджет при смене функции
    }

    // Задать диапазон отображения по X
    void setXRange(double xmin, double xmax)
    {
        m_xMin = xmin;
        m_xMax = xmax;
        update();
    }

    // Задать диапазон отображения по Y
    void setYRange(double ymin, double ymax)
    {
        m_yMin = ymin;
        m_yMax = ymax;
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
        painter.fillRect(rect(), Qt::white);

        // Всегда рисуем сетку и оси
        drawGrid(painter);
        drawAxes(painter);

        if (!m_function) {
            painter.setPen(Qt::black);

            // Устанавливаем шрифт Cambria размером 12
            QFont font("Cambria", 12);
            painter.setFont(font);

            // Получаем высоту текста
            QString text = "Функция не задана";
            QFontMetrics fm = painter.fontMetrics();
            int textHeight = fm.height();

            // Рисуем текст над графиком, с небольшим отступом сверху
            QRect textRect = rect();
            textRect.setHeight(textHeight);
            painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, text);

            return;
        }


        drawFunction(painter);
    }


private:
    Function* m_function;

    double m_xMin, m_xMax;
    double m_yMin, m_yMax;

    const int margin = 40; // отступы для осей

    // Преобразуем координату X из математической (в диапазоне [m_xMin, m_xMax])
    // в координату окна (пиксели)
    int mapX(double x) const
    {
        double w = width() - 2 * margin;
        return margin + int((x - m_xMin) / (m_xMax - m_xMin) * w);
    }

    // Преобразуем координату Y из математической (в диапазоне [m_yMin, m_yMax])
    // в координату окна (пиксели)
    int mapY(double y) const
    {
        double h = height() - 2 * margin;
        // Учёт, что в системе координат Qt Y растёт вниз
        return margin + int((m_yMax - y) / (m_yMax - m_yMin) * h);
    }

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
        painter.setFont(QFont("Cambria", 10));

        // Отметки по X
        int n = 10; // число промежутков
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
            painter.drawLine(mapX(0) - 5, yPos, mapX(0) + 5, yPos);
            painter.drawText(mapX(0) - 35, yPos + 5, QString::number(yVal, 'f', 1));
        }
    }

    void drawGrid(QPainter& painter)
    {
        painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));

        int n = 10; // число сеточных линий по каждой оси

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

    void drawFunction(QPainter& painter)
    {
        painter.setPen(QPen(Qt::blue, 2));

        // Рисуем функцию сглаженными линиями
        // Берём точки с шагом по X, равным 1 пикселю окна, преобразуем в координаты функции

        int w = width() - 2 * margin;
        if (w <= 0)
            return;

        QVector<QPoint> points;
        points.reserve(w);

        for (int i = 0; i <= w; ++i) {
            double x = m_xMin + (m_xMax - m_xMin) * i / w;
            double y = m_function->evaluate(x);

            // Игнорируем точки вне диапазона Y (для отрезков рисования)
            if (y < m_yMin || y > m_yMax)
                points.append(QPoint(mapX(x), -1)); // помечаем как «пропуск»
            else
                points.append(QPoint(mapX(x), mapY(y)));
        }

        // Рисуем линии между точками, игнорируя пропуски
        for (int i = 1; i < points.size(); ++i) {
            if (points[i - 1].y() >= 0 && points[i].y() >= 0) {
                painter.drawLine(points[i - 1], points[i]);
            }
        }
    }
};

#endif // GRAPHICWIDGET_H
