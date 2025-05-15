#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("F:/Dasha/Repos/GraphicEditor/Иконка.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Обработка подсказки
void MainWindow::on_pushButton_4_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Подсказка по вводу");

    QString text = R"(
        <div style="
            text-align: left;
            font-family: 'Cambria Math';
            font-size: 12pt;
            color: white;
            padding: 0;
            margin-left: -5px;
        ">
        Многочлен: a0 + a1x + a2x^2 + ...<br>
        Тригонометрическая: a*sin(b*x+c)+d<br>
        Экспоненциальная: a*exp(b*x+c)+d<br>
        Логарифмическая: a*log_b(c*x+d)+e<br>
        Модуль: c*|a*x+b|+d
        </div>
    )";

    msgBox.setText(text);

    // Устанавливаем фон QMessageBox и начальный стиль кнопок
    msgBox.setStyleSheet(R"(
        QMessageBox {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                       stop:0 #FF2E4C, stop:1 #1E2A78);
        }
        QMessageBox QPushButton {
            background: transparent;
            color: white;
            border: 2px solid white;
            border-radius: 20px;
            padding: 11px;
            font-family: Cambria;
            font-size: 12px;
            min-width: 70px;
            min-height: 15px;
        }
        QMessageBox QPushButton:hover {
            border: 3px solid white;
            text-decoration: underline;
        }
    )");

    if (!msgBox.button(QMessageBox::Ok)) {
        msgBox.addButton(QMessageBox::Ok);
    }

    QAbstractButton *abstractOkButton = msgBox.button(QMessageBox::Ok);
    QPushButton *okButton = qobject_cast<QPushButton *>(abstractOkButton);
    msgBox.exec();
}

void MainWindow::on_pushButton_clicked()
{
    double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    bool okX = false, okY = false;

    double x = ui->lineEdit->text().toDouble(&okX);
    if (okX) {
        xMin = -x;
        xMax = x;
    }

    double y = ui->lineEdit_2->text().toDouble(&okY);
    if (okY) {
        yMin = -y;
        yMax = y;
    }

    if (!(okX && okY)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный ввод диапазона осей");
        return;
    }

    if (!rangeController) {
        rangeController = new RangeController(this);
        connect(rangeController, &RangeController::rangeChanged,
                ui->graphicWidget, &::GraphicWidget::setRange);
    }

    rangeController->setRange(xMin, xMax, yMin, yMax);

    QString input = ui->lineEdit_4->text().trimmed();

    if (input.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пустой ввод функции");
        return;
    }

    auto parser = ParserFactory::createParser(input);
    if (!parser) {
        QMessageBox::warning(this, "Ошибка", "Не удалось определить тип функции");
        return;
    }

    Function* parsedFunc = parser->parse(input);
    if (!parsedFunc) {
        QMessageBox::warning(this, "Ошибка", "Некорректный ввод функции");
        return;
    }

    ui->graphicWidget->setMainFunction(parsedFunc, color1);
}

void MainWindow::on_pushButton_6_clicked()
{
    if (ui->graphicWidget->functionsCount() == 0) {
        QMessageBox::warning(this, "Ошибка", "Сначала постройте основной график!");
        return;
    }

    double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    bool okX = false, okY = false;

    double x = ui->lineEdit->text().toDouble(&okX);
    if (okX) {
        xMin = -x;
        xMax = x;
    }

    double y = ui->lineEdit_2->text().toDouble(&okY);
    if (okY) {
        yMin = -y;
        yMax = y;
    }

    if (!(okX && okY)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный ввод диапазона осей");
        return;
    }

    if (!rangeController) {
        rangeController = new RangeController(this);
        connect(rangeController, &RangeController::rangeChanged,
                ui->graphicWidget, &::GraphicWidget::setRange);
    }

    rangeController->setRange(xMin, xMax, yMin, yMax);

    QString input = ui->lineEdit_4->text().trimmed();

    if (input.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пустой ввод функции");
        return;
    }

    auto parser = ParserFactory::createParser(input);
    if (!parser) {
        QMessageBox::warning(this, "Ошибка", "Не удалось определить тип функции");
        return;
    }

    Function* parsedFunc = parser->parse(input);
    if (!parsedFunc) {
        QMessageBox::warning(this, "Ошибка", "Некорректный ввод функции");
        return;
    }

    ui->graphicWidget->setSecondaryFunction(parsedFunc, color2);
}

void MainWindow::on_pushButton_2_clicked()
{
    // Очищаем все графики на виджете
    ui->graphicWidget->clearFunctions();

    // Сбрасываем указатели на функции
    currentFunc1 = nullptr;
    currentFunc2 = nullptr;
}


void MainWindow::on_pushButton_3_clicked()
{
    // Получаем изображение из graphicWidget
    QPixmap pixmap = ui->graphicWidget->grab();

    // Сохраняем изображение в файл
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить изображение", "", "PNG Files (*.png);;All Files (*)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}

