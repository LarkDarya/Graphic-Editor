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

class LogarithmicParser {
public:
    static LogarithmicFunction* parse(const QString& input) {
        QString str = input.simplified().replace(" ", "");

        // Обработка дробей в выражении
        str = replaceFractions(str);

        // Основной шаблон для всех случаев
        QRegularExpression re(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?\+?)?([+-]?(?:\d+\.?\d*|\.\d+)?)\*?(log_(\d+\.?\d*|\(\d+/\d+\))\(([^)]+)\)|ln\(([^)]+)\)|log\(([^)]+)\))([+-]\d+\.?\d*)?$)");
        QRegularExpressionMatch match = re.match(str);

        if (!match.hasMatch()) {
            return nullptr;
        }

        // Парсим коэффициенты
        double d = 0.0, a = 1.0, base = 10.0, c = 1.0, e = 0.0;

        // Обработка d (свободного члена)
        if (!match.captured(1).isEmpty()) {
            QString dStr = match.captured(1);
            if (dStr.endsWith('+')) dStr.chop(1);
            if (!dStr.isEmpty()) d = dStr.toDouble();
        }

        // Обработка a (коэффициента перед логарифмом)
        if (!match.captured(2).isEmpty()) {
            QString aStr = match.captured(2);
            a = parseCoefficient(aStr);
        } else if (str.startsWith('-')) {
            a = -1.0;
        }

        // Определение типа логарифма и его основания
        if (!match.captured(4).isEmpty()) { // log_b(...)
            base = evalExpression(match.captured(4));
            parseInnerExpression(match.captured(5), c, e);
        }
        else if (!match.captured(6).isEmpty()) { // ln(...)
            base = M_E;
            parseInnerExpression(match.captured(6), c, e);
        }
        else if (!match.captured(7).isEmpty()) { // log(...)
            base = 10.0;
            parseInnerExpression(match.captured(7), c, e);
        }

        // Обработка дополнительного слагаемого (e)
        if (!match.captured(8).isEmpty()) {
            e += match.captured(8).toDouble();
        }

        // Создаем и возвращаем функцию
        LogarithmicFunction* func = new LogarithmicFunction();
        func->setCoefficients({a, base, c, d, e});
        return func;
    }


private:
    static double parseCoefficient(const QString& coeff) {
        if (coeff == "+") return 1.0;
        if (coeff == "-") return -1.0;
        return coeff.toDouble();
    }


    static void parseInnerExpression(const QString& expr, double& c, double& e) {
        if (expr.isEmpty()) return;

        QRegularExpression innerRe(R"(([+-]?\d*\.?\d*)\*?x([+-]\d+\.?\d*)?)");
        QRegularExpressionMatch innerMatch = innerRe.match(expr);

        if (innerMatch.hasMatch()) {
            QString cStr = innerMatch.captured(1);
            QString eStr = innerMatch.captured(2);

            if (!cStr.isEmpty()) c = cStr.isEmpty() ? 1.0 : cStr.toDouble();
            if (!eStr.isEmpty()) e = eStr.toDouble();
        } else {
            // Если нет x, то это просто константа e
            e = expr.toDouble();
            c = 0.0;
        }
    }

    static QString replaceFractions(const QString& expr) {
        QString result = expr;
        QRegularExpression fracRe(R"(\((\d+)/(\d+)\))");
        QRegularExpressionMatchIterator i = fracRe.globalMatch(expr);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            double num = match.captured(1).toDouble();
            double den = match.captured(2).toDouble();
            if (den != 0) {
                double value = num / den;
                result.replace(match.captured(0), QString::number(value, 'g', 10));
            }
        }
        return result;
    }

    static double evalExpression(const QString& expr) {
        if (expr.isEmpty()) return 1.0;
        if (expr.contains('/')) {
            QStringList parts = expr.split('/');
            if (parts.size() == 2) {
                double num = parts[0].toDouble();
                double den = parts[1].toDouble();
                return den != 0 ? num / den : 1.0;
            }
        }
        return expr.toDouble();
    }
};

// Парсер для модульных функций
class ModulusParser {
public:
    static ModulusFunction* parse(const QString& input) {
        QString str = input.simplified().replace(" ", "");

        // Общий шаблон: d + c*|a*x + b|
        QRegularExpression re(R"(^([+-]?\d*\.?\d*)\+?([+-]?\d*\.?\d*)\*?\|([+-]?\d*\.?\d*)\*?x([+-]\d+\.?\d*)?\|$)");
        QRegularExpressionMatch match = re.match(str);

        if (!match.hasMatch()) {
            // Попробуем вариант без смещения d: c*|a*x + b|
            re.setPattern(R"(^([+-]?\d*\.?\d*)\*?\|([+-]?\d*\.?\d*)\*?x([+-]\d+\.?\d*)?\|$)");
            match = re.match(str);

            if (!match.hasMatch()) {
                // Попробуем простые случаи
                if (str == "|x|") {
                    ModulusFunction* func = new ModulusFunction();
                    func->setCoefficients({0.0, 0.0, 1.0, 1.0});
                    return func;
                }
                else if (str == "|-x|") {
                    ModulusFunction* func = new ModulusFunction();
                    func->setCoefficients({0.0, 0.0, -1.0, 1.0});
                    return func;
                }
                else if (str == "-|x|") {
                    ModulusFunction* func = new ModulusFunction();
                    func->setCoefficients({0.0, 0.0, 1.0, -1.0});
                    return func;
                }
                return nullptr;
            }

            // Обработка c*|a*x + b|
            double c = match.captured(1).isEmpty() ? 1.0 : match.captured(1).toDouble();
            double a = match.captured(2).isEmpty() ? 1.0 : match.captured(2).toDouble();
            double b = match.captured(3).isEmpty() ? 0.0 : match.captured(3).toDouble();

            ModulusFunction* func = new ModulusFunction();
            func->setCoefficients({b, 0.0, a, c});
            return func;
        }

        // Обработка полной формы: d + c*|a*x + b|
        double d = match.captured(1).isEmpty() ? 0.0 : match.captured(1).toDouble();
        double c = match.captured(2).isEmpty() ? 1.0 : match.captured(2).toDouble();
        double a = match.captured(3).isEmpty() ? 1.0 : match.captured(3).toDouble();
        double b = match.captured(4).isEmpty() ? 0.0 : match.captured(4).toDouble();

        ModulusFunction* func = new ModulusFunction();
        func->setCoefficients({b, d, a, c});
        return func;
    }
};

#endif // PARSER_H
