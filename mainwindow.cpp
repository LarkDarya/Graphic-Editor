#include "mainwindow.h"
#include "QMessageBox.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("F:/Даша/Репозиторий/GraphicEditor/Иконка.png"));
    // Изначально делаем label_3 и lineEdit_3 неактивными и "сероватыми"
    ui->label_3->setEnabled(false);
    ui->lineEdit_3->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Обработка шага (если функция не тригонометрическая, то шаг не используется)
void MainWindow::on_radioButton_2_toggled(bool checked)
{
    ui->label_3->setEnabled(checked);
    ui->lineEdit_3->setEnabled(checked);

    if (checked) {
        ui->label_3->setStyleSheet("background: transparent;color: white;font-family: Cambria;font-size: 20px;");
        ui->lineEdit_3->setStyleSheet("background: transparent;color: white; border: 2px solid white;");
    } else {
        ui->label_3->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
        ui->lineEdit_3->clear();

        ui->label_3->setStyleSheet("background: #f0f0f0;color: gray;font-family: Cambria;font-size: 20px;");
        ui->lineEdit_3->setStyleSheet("background: #f0f0f0;");
    }
}

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
            min-width: 70px;         /* минимальная ширина кнопки */
            min-height: 15px;
        }
        QMessageBox QPushButton:hover {
            border: 3px solid white;
            text-decoration: underline;
        }
    )");

    // Добавляем кнопку ОК, если её нет
    if (!msgBox.button(QMessageBox::Ok)) {
        msgBox.addButton(QMessageBox::Ok);
    }

    // Приводим тип и можно дополнительно работать с кнопкой, если надо
    QAbstractButton *abstractOkButton = msgBox.button(QMessageBox::Ok);
    QPushButton *okButton = qobject_cast<QPushButton *>(abstractOkButton);

    if (okButton) {
        // Можно здесь настроить что-то дополнительно для okButton, если нужно
    }

    msgBox.exec();
}




