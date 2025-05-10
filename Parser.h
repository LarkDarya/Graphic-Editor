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
// Простая реализация, разбивающая строку на части

class ModulusParser {
public:
    static ModulusFunction* parse(const QString& input) {
        QString str = input.simplified().replace(" ", "");

        // Сначала ищем смещение по y (d) в конце вида + или - число
        QRegularExpression dExpr(R"(([+-]\d+(\.\d+)?)$)");
        QRegularExpressionMatch dMatch = dExpr.match(str);

        double d = 0.0;
        if (dMatch.hasMatch()) {
            QString dStr = dMatch.captured(1);
            d = dStr.toDouble();
            str.chop(dStr.length()); // Убираем смещение d с конца
        }

        // Теперь форматы типа "2*|x|", "|x|", "-|x|", "3*|2*x+1|"

        // Парсим множитель c перед модулем
        double c = 1.0;
        QRegularExpression cExpr(R"(^([+-]?\d*\.?\d*)\*?\|)");
        QRegularExpressionMatch cMatch = cExpr.match(str);
        if (cMatch.hasMatch()) {
            QString cStr = cMatch.captured(1);
            if (!cStr.isEmpty() && cStr != "+" && cStr != "-")
                c = cStr.toDouble();
            else if (cStr == "-")
                c = -1.0;
            // удаляем из строки множитель c и начальный '|'
            int pos = cMatch.capturedLength();
            str = str.mid(pos - 1); // сохранить '|...' (сдвинем обратно на 1)
        }

        // Теперь внутри модуля ожидается что-то типа "a*x+b" или просто "x"
        QRegularExpression insideExpr(R"(\|([+-]?\d*\.?\d*)\*?x([+-]\d+(\.\d+)?)?\|)");
        QRegularExpressionMatch insideMatch = insideExpr.match(str);
        double a = 1.0;
        double b = 0.0;
        if (insideMatch.hasMatch()) {
            QString aStr = insideMatch.captured(1);
            QString bStr = insideMatch.captured(2);

            if (!aStr.isEmpty() && aStr != "+" && aStr != "-")
                a = aStr.toDouble();
            else if (aStr == "-")
                a = -1.0;

            if (!bStr.isEmpty())
                b = bStr.toDouble();
        }
        else {
            // Может быть просто |x|
            if (str == "|x|")
                a = 1.0, b = 0.0;
            else if (str == "|-x|")
                a = -1.0, b = 0.0;
            else
                return nullptr; // не распознан формат
        }

        ModulusFunction* func = new ModulusFunction();
        func->setCoefficients({b, d, a, c});
        return func;
    }
};



#endif // PARSER_H
