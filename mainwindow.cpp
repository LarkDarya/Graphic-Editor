#include "mainwindow.h"
#include "QMessageBox.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("F:/Даша/Репозиторий/GraphicEditor/Иконка.png"));
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
                ui->graphicWidget, &GraphicWidget::setRange);
    }

    rangeController->setRange(xMin, xMax, yMin, yMax);

    if (ui->radioButton->isChecked()) {
        QString input = ui->lineEdit_4->text();
        QVector<double> coeffs = PolynomialParser::parse(input);
        polyFunc.setCoefficients(coeffs);
        ui->graphicWidget->setFunction(&polyFunc);
    }
    else if (ui->radioButton_2->isChecked()) {
        QString input = ui->lineEdit_4->text();
        TrigonometricFunction* parsedFunc = TrigonometricParser::parse(input);
        if (parsedFunc) {
            trigFunc.setType(parsedFunc->getType());
            trigFunc.setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setFunction(&trigFunc);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить тригонометрическую функцию");
        }
    }
    else if (ui->radioButton_3->isChecked()) {
        QString input = ui->lineEdit_4->text();
        ExponentialFunction* parsedFunc = ExponentialParser::parse(input);
        if (parsedFunc) {
            expFunc.setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setFunction(&expFunc);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить функцию экспоненты");
        }
    }
    else if (ui->radioButton_4->isChecked()) {
        QString input = ui->lineEdit_4->text();
        LogarithmicFunction* parsedFunc = LogarithmicParser::parse(input);
        if (parsedFunc) {
            logFunc.setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setFunction(&logFunc);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить логарифмическую функцию");
        }
    }
    else if (ui->radioButton_9->isChecked()) {
        QString input = ui->lineEdit_4->text();
        ModulusFunction* parsedFunc = ModulusParser::parse(input);
        if (parsedFunc) {
            modFunc.setCoefficients(parsedFunc->getCoefficients());
            ui->graphicWidget->setFunction(&modFunc);
            delete parsedFunc;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось распарсить модульную функцию");
        }
    }
    else {
        QMessageBox::warning(this, "Ошибка", "Не выбрана функция");
        return;
    }

    ui->graphicWidget->update();
}
