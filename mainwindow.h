#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graphicwidget.h"
#include "RangeController.h"
#include "Parser.h"
#include "Function.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
    void on_pushButton_6_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    GraphicWidget* graphicWidget;
    PolynomialFunction polyFunc;
    TrigonometricFunction trigFunc;
    ExponentialFunction expFunc;
    LogarithmicFunction logFunc;
    ModulusFunction modFunc;
    RangeController* rangeController = nullptr;

    QColor getNextColor();

    Function* currentFunc1 = nullptr; // Для первого графика
    Function* currentFunc2 = nullptr; // Для второго графика
    QColor color1 = Qt::blue;         // Цвет первого графика
    QColor color2 = Qt::red;


};
#endif // MAINWINDOW_H
