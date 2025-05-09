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

// Парсер для тригонометрических функций
class TrigonometricParser {
public:
    static TrigonometricFunction* parse(const QString& input) {
        QString str = input.simplified().replace(" ", "");

        // Общий шаблон для тригонометрических функций: a*sin(b*x + c) + d
        QRegularExpression re(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?(sin|cos|tan|cot)\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
        QRegularExpressionMatch match = re.match(str);

        if (!match.hasMatch()) {
            // Проверим вариант без коэффициента перед функцией (-sin(x))
            re.setPattern(R"(^([+-])?(sin|cos|tan|cot)\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
            match = re.match(str);
            if (!match.hasMatch()) {
                return nullptr;
            }
        }

        TrigonometricFunction::Type type;
        QString funcType = match.captured(2);
        if (funcType == "sin") type = TrigonometricFunction::Sin;
        else if (funcType == "cos") type = TrigonometricFunction::Cos;
        else if (funcType == "tan") type = TrigonometricFunction::Tan;
        else if (funcType == "cot") type = TrigonometricFunction::Cot;
        else return nullptr;

        double a = 1.0;
        // Обработка отрицательного знака перед функцией
        if (match.captured(1) == "-") {
            a = -1.0;
        } else if (!match.captured(1).isEmpty() && match.captured(1) != "+") {
            a = match.captured(1).toDouble();
        }

        double b = match.captured(3).isEmpty() ? 1.0 : match.captured(3).toDouble();
        double c = match.captured(4).isEmpty() ? 0.0 : match.captured(4).toDouble();
        double d = match.captured(5).isEmpty() ? 0.0 : match.captured(5).toDouble();

        TrigonometricFunction* func = new TrigonometricFunction(type);
        func->setCoefficients({d, a, b, c});
        return func;
    }
};

// Парсер для экспоненциальных функций
class ExponentialParser {
public:
    static ExponentialFunction* parse(const QString& input) {
        QString str = input.simplified().replace(" ", "");

        // Шаблон для функций вида a*exp(b*x + c) + d
        QRegularExpression re(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?exp\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
        QRegularExpressionMatch match = re.match(str);

        if (!match.hasMatch()) {
            // Проверим вариант без коэффициента перед функцией (-exp(x))
            re.setPattern(R"(^([+-])?exp\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
            match = re.match(str);
            if (!match.hasMatch()) {
                return nullptr;
            }
        }

        double a = 1.0;
        // Обработка отрицательного знака перед функцией
        if (match.captured(1) == "-") {
            a = -1.0;
        } else if (!match.captured(1).isEmpty() && match.captured(1) != "+") {
            a = match.captured(1).toDouble();
        }

        double b = match.captured(2).isEmpty() ? 1.0 : match.captured(2).toDouble();
        double c = match.captured(3).isEmpty() ? 0.0 : match.captured(3).toDouble();
        double d = match.captured(4).isEmpty() ? 0.0 : match.captured(4).toDouble();

        ExponentialFunction* func = new ExponentialFunction();
        func->setCoefficients({d, a, b, c});
        return func;
    }
};

// Парсер для логарифмических функций
class LogarithmicParser {
public:
    static LogarithmicFunction* parse(const QString& input) {
        QString str = input.simplified().replace(" ", "");

        // Шаблон для функций вида a*log_b(x)
        QRegularExpression re(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?log_(\d+\.?\d*)\(x\)$)");
        QRegularExpressionMatch match = re.match(str);

        if (!match.hasMatch()) {
            // Попробуем натуральный логарифм
            re.setPattern(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?ln\(x\)$)");
            match = re.match(str);
            if (!match.hasMatch()) {
                // Попробуем простой log(x) (по основанию 10)
                re.setPattern(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?log\(x\)$)");
                match = re.match(str);
                if (!match.hasMatch()) {
                    return nullptr;
                }
                double a = match.captured(1).isEmpty() ? 1.0 : match.captured(1).toDouble();
                LogarithmicFunction* func = new LogarithmicFunction();
                func->setCoefficients({a, 10.0}); // десятичный логарифм
                return func;
            }

            double a = match.captured(1).isEmpty() ? 1.0 : match.captured(1).toDouble();
            LogarithmicFunction* func = new LogarithmicFunction();
            func->setCoefficients({a, M_E}); // натуральный логарифм
            return func;
        }

        double a = match.captured(1).isEmpty() ? 1.0 : match.captured(1).toDouble();
        double base = match.captured(2).toDouble();

        LogarithmicFunction* func = new LogarithmicFunction();
        func->setCoefficients({a, base});
        return func;
    }
};

// Парсер для модульных функций
class ModulusParser {
public:
    static ModulusFunction* parse(const QString& input) {
        QString str = input.simplified().replace(" ", "");

        // Шаблон для функций вида |x + a| + b
        QRegularExpression re(R"(^\|x([+-]\d+\.?\d*)?\|([+-]\d+\.?\d*)?$)");
        QRegularExpressionMatch match = re.match(str);

        if (!match.hasMatch()) {
            // Проверим простой случай |x|
            if (str == "|x|") {
                ModulusFunction* func = new ModulusFunction();
                func->setCoefficients({0.0, 0.0});
                return func;
            }
            return nullptr;
        }

        double a = match.captured(1).isEmpty() ? 0.0 : match.captured(1).toDouble();
        double b = match.captured(2).isEmpty() ? 0.0 : match.captured(2).toDouble();

        ModulusFunction* func = new ModulusFunction();
        func->setCoefficients({a, b});
        return func;
    }
};

#endif // PARSER_H
