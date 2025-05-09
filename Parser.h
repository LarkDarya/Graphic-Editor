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

        // Определяем тип логарифма и основание
        struct LogInfo {
            QString pattern;
            double defaultBase;
        };

        QVector<LogInfo> logTypes = {
            {"log_", 10.0}, // Основание будет извлечено
            {"ln(", M_E},
            {"log(", 10.0}
        };

        QString matchedType;
        double base = 10.0;
        QString innerExpr;

        // Ищем совпадение с типами логарифмов
        for (const auto& lt : logTypes) {
            if (str.contains(lt.pattern)) {
                matchedType = lt.pattern;
                base = lt.defaultBase;

                // Для log_ извлекаем основание
                if (lt.pattern == "log_") {
                    QRegularExpression baseRe(R"(log_(\d+\.?\d*|\(\d+/\d+\))\()");
                    QRegularExpressionMatch baseMatch = baseRe.match(str);
                    if (baseMatch.hasMatch()) {
                        QString baseStr = baseMatch.captured(1);
                        if (baseStr.contains('/')) {
                            base = evalFraction(baseStr);
                        } else {
                            base = baseStr.toDouble();
                        }
                    }
                }

                // Извлекаем внутреннее выражение
                int start = str.indexOf(matchedType) + matchedType.length();
                int end = str.indexOf(")", start);
                if (end == -1) return nullptr;
                innerExpr = str.mid(start, end - start);
                break;
            }
        }

        if (matchedType.isEmpty()) return nullptr;

        // Парсим коэффициенты
        double a = 1.0, c = 1.0, e = 0.0, d = 0.0;

        // Парсим выражение перед логарифмом (d + a*)
        QString prefix = str.left(str.indexOf(matchedType));
        if (!prefix.isEmpty()) {
            // Обработка дробей и сложных выражений
            if (prefix.contains('/') || prefix.contains('+') || prefix.contains('-')) {
                QRegularExpression prefixRe(R"(([^+]*)\+?([^+]*)\*?)");
                QRegularExpressionMatch prefixMatch = prefixRe.match(prefix);

                if (prefixMatch.hasMatch()) {
                    QString dStr = prefixMatch.captured(1);
                    QString aStr = prefixMatch.captured(2);

                    if (!dStr.isEmpty()) d = evalExpression(dStr);
                    if (!aStr.isEmpty()) a = evalExpression(aStr);
                }
            } else {
                // Простые случаи
                if (prefix == "-") a = -1.0;
                else if (prefix == "+") a = 1.0;
                else if (prefix.endsWith("*")) a = evalExpression(prefix.left(prefix.length()-1));
                else a = evalExpression(prefix);
            }
        }

        // Парсим внутреннее выражение (c*x + e)
        if (!innerExpr.isEmpty()) {
            innerExpr = innerExpr.replace(" ", "");

            if (innerExpr.contains("x")) {
                QRegularExpression innerRe(R"(([^x]*)x([^+]*)");
                QRegularExpressionMatch innerMatch = innerRe.match(innerExpr);

                if (innerMatch.hasMatch()) {
                    QString cStr = innerMatch.captured(1);
                    QString eStr = innerMatch.captured(2);

                    if (!cStr.isEmpty()) c = evalExpression(cStr.isEmpty() ? "1" : cStr);
                    if (!eStr.isEmpty()) e = evalExpression(eStr);
                }
            } else {
                e = evalExpression(innerExpr);
                c = 0.0;
            }
        }

        // Создаем и возвращаем функцию
        LogarithmicFunction* func = new LogarithmicFunction();
        func->setCoefficients({a, base, c, d, e});
        return func;
    }

private:
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

    static double evalFraction(const QString& frac) {
        QStringList parts = frac.split('/');
        if (parts.size() == 2) {
            double num = parts[0].toDouble();
            double den = parts[1].toDouble();
            return den != 0 ? num / den : 1.0;
        }
        return 1.0;
    }

    static double evalExpression(const QString& expr) {
        if (expr.isEmpty()) return 1.0;
        if (expr == "-") return -1.0;
        if (expr == "+") return 1.0;
        if (expr.contains('/')) return evalFraction(expr);
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
