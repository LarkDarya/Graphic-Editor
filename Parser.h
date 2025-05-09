#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QVector>
#include <QRegularExpression>
#include <QDebug>
#include "Function.h"

class PolynomialParser {
public:
    static QVector<double> parse(const QString& input) {
        QString line = input;
        line.remove(' ');
        // Берём слагаемые с возможным дробным коэффициентом
        QRegularExpression re(R"(([+-]?)(\d+(?:/\d+)?|\d*\.?\d*)\*?x?(\^(\d+))?)");

        QRegularExpressionMatchIterator i = re.globalMatch(line);

        int maxDegree = 0;
        QVector<std::pair<int, double>> terms;

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();

            QString signStr = match.captured(1);
            QString coeffStr = match.captured(2);
            QString powerStr = match.captured(4);

            double sign = (signStr == "-") ? -1.0 : 1.0;

            bool hasX = match.captured(0).contains('x');

            double coeff = 0.0;

            if (coeffStr.isEmpty()) {
                coeff = hasX ? 1.0 : 0.0;
            } else {
                // Проверяем, есть ли дробь
                if (coeffStr.contains('/')) {
                    QStringList parts = coeffStr.split('/');
                    if (parts.size() == 2) {
                        bool ok1, ok2;
                        double numerator = parts[0].toDouble(&ok1);
                        double denominator = parts[1].toDouble(&ok2);
                        if (ok1 && ok2 && denominator != 0)
                            coeff = numerator / denominator;
                        else
                            coeff = 0.0;
                    } else {
                        coeff = 0.0;
                    }
                } else {
                    coeff = coeffStr.toDouble();
                }
            }

            coeff *= sign;

            int power = 0;
            if (hasX) {
                power = powerStr.isEmpty() ? 1 : powerStr.toInt();
            }

            if (power > maxDegree)
                maxDegree = power;

            terms.append({power, coeff});
        }

        QVector<double> coefs(maxDegree + 1, 0.0);
        for (auto& term : terms) {
            coefs[term.first] += term.second;
        }

        return coefs;
    }
};

class TrigonometricParser {
public:
    // Возвращает nullptr при ошибке, иначе новый TrigonometricFunction с заданными коэффициентами
    static TrigonometricFunction* parse(const QString& input) {
        QString line = input;
        line.remove(' ');
        // Регулярка для парсинга строк вида: (коэффициент)? (sin|cos|tan|cot) ( (x (+|-)? число | дробь)? )
        // Пример: "2*sin(x+0.5)", "-0.5*cos(x-1/2)", "tan(x)", "cot(x+1)"
        QRegularExpression re(R"(^([+-]?(\d+(\.\d+)?|(\d+/\d+)))?\*?(sin|cos|tan|cot)\(x([+-](\d+(\.\d+)?|\d+/\d+))?\)$)", QRegularExpression::CaseInsensitiveOption);

        QRegularExpressionMatch match = re.match(line);
        if (!match.hasMatch()) {
            return nullptr; // ошибка парсинга
        }

        // Коэффициент амплитуды (опционально)
        QString ampStr = match.captured(1);
        double amplitude = 1.0;
        if (!ampStr.isEmpty()) {
            amplitude = parseFractionOrDouble(ampStr);
        }

        // Тип функции
        QString funcStr = match.captured(5).toLower();
        TrigonometricFunction::Type type;
        if (funcStr == "sin") type = TrigonometricFunction::Sin;
        else if (funcStr == "cos") type = TrigonometricFunction::Cos;
        else if (funcStr == "tan") type = TrigonometricFunction::Tan;
        else if (funcStr == "cot") type = TrigonometricFunction::Cot;
        else return nullptr; // неизвестный тип

        // Сдвиг
        QString shiftStr = match.captured(6);
        double shift = 0.0;
        if (!shiftStr.isEmpty()) {
            shift = parseFractionOrDouble(shiftStr.mid(1)); // убираем первый символ '+' или '-'
            if (shiftStr[0] == '-') shift = -shift;
        }

        TrigonometricFunction* trigFunc = new TrigonometricFunction(type);
        trigFunc->setCoefficients({amplitude, shift});
        return trigFunc;
    }

private:
    // Парсер дроби (например 1/2) или десятичного числа
    static double parseFractionOrDouble(const QString& str) {
        if (str.contains('/')) {
            QStringList parts = str.split('/');
            if (parts.size() == 2) {
                bool okNum, okDen;
                double num = parts[0].toDouble(&okNum);
                double den = parts[1].toDouble(&okDen);
                if (okNum && okDen && den != 0) {
                    return num / den;
                }
                return 0.0;
            }
            return 0.0;
        } else {
            bool ok;
            double val = str.toDouble(&ok);
            return ok ? val : 0.0;
        }
    }
};


#endif // PARSER_H
