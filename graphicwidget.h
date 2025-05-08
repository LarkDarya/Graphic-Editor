#ifndef GRAPHICWIDGET_H
#define GRAPHICWIDGET_H

#include <QWidget>
#include <QPainter>

class GraphicWidget : public QWidget {
    Q_OBJECT

public:
    explicit GraphicWidget(QWidget *parent = nullptr) : QWidget(parent) {
        // Можно задать размер виджета по умолчанию
        setMinimumSize(400, 300);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);

        painter.fillRect(rect(), Qt::white);

        int margin = 50;
        int w = width() - margin * 2;
        int h = height() - margin * 2;

        // Настройки пера для осей
        QPen axisPen(Qt::black, 2);
        painter.setPen(axisPen);

        // Рисуем оси X и Y с стрелками
        // Ось X
        painter.drawLine(margin, height() - margin, width() - margin, height() - margin);
        painter.drawLine(width() - margin, height() - margin, width() - margin - 10, height() - margin - 5);
        painter.drawLine(width() - margin, height() - margin, width() - margin - 10, height() - margin + 5);

        // Ось Y
        painter.drawLine(margin, margin, margin, height() - margin);
        painter.drawLine(margin, margin, margin - 5, margin + 10);
        painter.drawLine(margin, margin, margin + 5, margin + 10);

        // Рисуем сетку (горизонтальные линии)
        QPen gridPen(Qt::gray, 1, Qt::DashLine);
        painter.setPen(gridPen);

        int steps = 10;
        for (int i = 0; i <= steps; ++i) {
            int y = margin + i * h / steps;
            painter.drawLine(margin, y, margin + w, y);
        }

        // Подписи на осях
        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);

        // Ось X - подписи 0 .. 4π
        for (int i = 0; i <= steps; ++i) {
            int x = margin + i * w / steps;
            QString label = QString::number(i * 4.0 * 3.1415 / steps, 'f', 1);
            painter.drawText(x - 10, height() - margin + 20, label);
        }

        // Ось Y - подписи -1, -0.5, 0, 0.5, 1
        for (int i = 0; i <= 4; ++i) {
            int y = margin + i * h / 4;
            QString label = QString::number(1.0 - i*0.5, 'f', 1);
            painter.drawText(margin - 30, y + 5, label);
        }

        // Рисуем график функции y = sin(x)
        painter.setPen(QPen(Qt::blue, 2));
        QPoint prevPoint;

        for (int i = 0; i <= w; i++) {
            double x = (double)i / w * 4 * 3.1415; // 0..4π
            double y = sin(x);

            int px = margin + i;
            int py = margin + h/2 - (y * h/2);

            QPoint point(px, py);
            if (i > 0)
                painter.drawLine(prevPoint, point);
            prevPoint = point;
        }

        // Легенда
        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);
        painter.drawRect(width() - margin - 100, margin + 10, 20, 10);
        painter.drawText(width() - margin - 70, margin + 20, "y = sin(x)");
    }
};

#endif // GRAPHICWIDGET_H
