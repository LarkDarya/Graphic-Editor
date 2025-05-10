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
        Многочлен: a0 + a1*x + a2*x^2 + ...<br>
        Тригонометрическая: a*sin(b*x)<br>
        Экспоненциальная: a*exp(b*x)<br>
        Логарифмическая: a*log_b(x)<br>
        Модуль: |x + a| + b
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

void MainWindow::on_lineEdit_4_textEdited(const QString &arg1)
{
    if (ui->radioButton->isChecked()) {
        // Парсим полиномиальную функцию из строки
        QVector<double> coefs = PolynomialParser::parse(arg1);
        polyFunc.setCoefficients(coefs);
    }

    if (ui->radioButton_2->isChecked()) {
        // Получаем текущий текст из lineEdit_4 (аргумент arg1 в вашем слоте тоже можно использовать)
        QString input = ui->lineEdit_4->text();

        // Парсим строку через ваш парсер тригонометрических функций
        TrigonometricFunction* parsedFunc = TrigonometricParser::parse(input);

        if (parsedFunc) {
            // Устанавливаем тип и коэффициенты из результата парсинга
            trigFunc.setType(parsedFunc->getType());
            trigFunc.setCoefficients(parsedFunc->getCoefficients());

            delete parsedFunc; // освобождаем память
        }

    }

}

void MainWindow::on_pushButton_clicked()
{
    // Проверяем диапазон
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

    // Создаем и добавляем первый график
    if (ui->radioButton->isChecked()) {
        QString input = ui->lineEdit_4->text();
        QVector<double> coeffs = PolynomialParser::parse(input);
        Function* newFunc = new PolynomialFunction();
        newFunc->setCoefficients(coeffs);
        ui->graphicWidget->setMainFunction(newFunc, color1);
    }
    else if (ui->radioButton_2->isChecked()) {
        QString input = ui->lineEdit_4->text();
        TrigonometricFunction* parsedFunc = TrigonometricParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new TrigonometricFunction(parsedFunc->getType());
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setMainFunction(newFunc, color1);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить тригонометрическую функцию");
        }
    }
    else if (ui->radioButton_3->isChecked()) {
        QString input = ui->lineEdit_4->text();
        ExponentialFunction* parsedFunc = ExponentialParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new ExponentialFunction();
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setMainFunction(newFunc, color1);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить функцию экспоненты");
        }
    }
    else if (ui->radioButton_4->isChecked()) {
        QString input = ui->lineEdit_4->text();
        LogarithmicFunction* parsedFunc = LogarithmicParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new LogarithmicFunction();
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setMainFunction(newFunc, color1);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить логарифмическую функцию");
        }
    }
    else if (ui->radioButton_9->isChecked()) {
        QString input = ui->lineEdit_4->text();
        ModulusFunction* parsedFunc = ModulusParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new ModulusFunction();
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setMainFunction(newFunc, color1);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить модульную функцию");
        }
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    // Проверяем диапазон
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

    // Создаем и добавляем второй график
    if (ui->radioButton->isChecked()) {
        QString input = ui->lineEdit_4->text();
        QVector<double> coeffs = PolynomialParser::parse(input);
        Function* newFunc = new PolynomialFunction();
        newFunc->setCoefficients(coeffs);
        ui->graphicWidget->setSecondaryFunction(newFunc, color2);
    }
    else if (ui->radioButton_2->isChecked()) {
        QString input = ui->lineEdit_4->text();
        TrigonometricFunction* parsedFunc = TrigonometricParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new TrigonometricFunction(parsedFunc->getType());
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setSecondaryFunction(newFunc, color2);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить тригонометрическую функцию");
        }
    }
    else if (ui->radioButton_3->isChecked()) {
        QString input = ui->lineEdit_4->text();
        ExponentialFunction* parsedFunc = ExponentialParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new ExponentialFunction();
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setSecondaryFunction(newFunc, color2);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить функцию экспоненты");
        }
    }
    else if (ui->radioButton_4->isChecked()) {
        QString input = ui->lineEdit_4->text();
        LogarithmicFunction* parsedFunc = LogarithmicParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new LogarithmicFunction();
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setSecondaryFunction(newFunc, color2);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить логарифмическую функцию");
        }
    }
    else if (ui->radioButton_9->isChecked()) {
        QString input = ui->lineEdit_4->text();
        ModulusFunction* parsedFunc = ModulusParser::parse(input);
        if (parsedFunc) {
            Function* newFunc = new ModulusFunction();
            newFunc->setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setSecondaryFunction(newFunc, color2);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить модульную функцию");
        }
    }
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

