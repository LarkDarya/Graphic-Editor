#include "Parser.h"
#include <cmath>

Function* PolynomialParser::parse(const QString& input) {
    QVector<double> coefficients = parseCoefficients(input);
    if (coefficients.isEmpty()) {
        return nullptr;
    }
    return new PolynomialFunction(coefficients);
}

QVector<double> PolynomialParser::parseCoefficients(const QString& input) {
    QString line = input;
    line.remove(' ');

    QRegularExpression validCharsRegex(R"(^[0-9xX+\-*/^.\s/]+$)");
    if (!validCharsRegex.match(line).hasMatch()) {
        return {};
    }

    QRegularExpression re(R"(([+-]?)(\d+(?:/\d+)?|\d*\.?\d*)\*?x?(\^(\d+))?)");
    QRegularExpressionMatchIterator i = re.globalMatch(line);

    int maxDegree = 0;
    QVector<std::pair<int, double>> terms;
    bool foundTerm = false;

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();

        if (match.captured(0).isEmpty()) continue;

        foundTerm = true;

        QString signStr = match.captured(1);
        QString coeffStr = match.captured(2);
        QString powerStr = match.captured(4);

        double sign = (signStr == "-") ? -1.0 : 1.0;
        bool hasX = match.captured(0).contains('x');
        double coeff = 0.0;

        if (coeffStr.isEmpty()) {
            coeff = hasX ? 1.0 : 0.0;
        } else {
            if (coeffStr.contains('/')) {
                QStringList parts = coeffStr.split('/');
                if (parts.size() == 2) {
                    bool ok1, ok2;
                    double numerator = parts[0].toDouble(&ok1);
                    double denominator = parts[1].toDouble(&ok2);
                    if (ok1 && ok2 && denominator != 0)
                        coeff = numerator / denominator;
                    else
                        return {};
                } else {
                    return {};
                }
            } else {
                bool okCoeff = false;
                coeff = coeffStr.toDouble(&okCoeff);
                if (!okCoeff) {
                    return {};
                }
            }
        }

        coeff *= sign;
        int power = 0;
        if (hasX) {
            bool okPower = false;
            power = powerStr.isEmpty() ? 1 : powerStr.toInt(&okPower);
            if (!okPower && !powerStr.isEmpty()) {
                return {};
            }
        }

        if (power > maxDegree)
            maxDegree = power;

        terms.append({power, coeff});
    }

    if (!foundTerm) {
        return {};
    }

    QVector<double> coefs(maxDegree + 1, 0.0);
    for (auto& term : terms) {
        coefs[term.first] += term.second;
    }

    return coefs;
}

Function* TrigonometricParser::parse(const QString& input) {
    QString str = input.simplified().replace(" ", "");

    QRegularExpression re(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?(sin|cos|tan|cot)\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]?\d*\.?\d*)?\)([+-]?\d*\.?\d*)?$)");
    QRegularExpressionMatch match = re.match(str);

    if (!match.hasMatch()) {
        re.setPattern(R"(^([+-])?(sin|cos|tan|cot)\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]?\d*\.?\d*)?\)([+-]?\d*\.?\d*)?$)");
        match = re.match(str);
        if (!match.hasMatch()) {
            re.setPattern(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?(sin|cos|tan|cot)\(-x\)([+-]?\d*\.?\d*)?$)");
            match = re.match(str);
            if (!match.hasMatch()) {
                re.setPattern(R"(^([+-])?(sin|cos|tan|cot)\(-x\)([+-]?\d*\.?\d*)?$)");
                match = re.match(str);
                if (!match.hasMatch()) {
                    return nullptr;
                }
            }
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
    if (match.captured(1) == "-") {
        a = -1.0;
    } else if (!match.captured(1).isEmpty() && match.captured(1) != "+") {
        a = match.captured(1).toDouble();
    }

    double b = 1.0;
    double c = 0.0;

    if (match.captured(0).contains("(-x)")) {
        b = -1.0;
    } else {
        b = match.captured(3).isEmpty() ? 1.0 : match.captured(3).toDouble();
        c = match.captured(4).isEmpty() ? 0.0 : match.captured(4).toDouble();
    }

    double d = match.captured(match.capturedTexts().size() - 1).isEmpty() ? 0.0 : match.captured(match.capturedTexts().size() - 1).toDouble();

    TrigonometricFunction* func = new TrigonometricFunction(type);
    func->setCoefficients({d, a, b, c});
    return func;
}

Function* ExponentialParser::parse(const QString& input) {
    QString str = input.simplified().replace(" ", "");

    QRegularExpression re(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?exp\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
    QRegularExpressionMatch match = re.match(str);

    if (!match.hasMatch()) {
        re.setPattern(R"(^([+-])?exp\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
        match = re.match(str);
        if (!match.hasMatch()) {
            return nullptr;
        }
    }

    double a = 1.0;
    if (match.captured(1) == "-") {
        a = -1.0;
    } else if (!match.captured(1).isEmpty() && match.captured(1) != "+") {
        a = match.captured(1).toDouble();
    }

    QString cap = match.captured(2);
    double b = (cap == "-") ? -1.0 : (cap.isEmpty() ? 1.0 : cap.toDouble());
    double c = match.captured(3).isEmpty() ? 0.0 : match.captured(3).toDouble();
    double d = match.captured(4).isEmpty() ? 0.0 : match.captured(4).toDouble();

    ExponentialFunction* func = new ExponentialFunction();
    func->setCoefficients({d, a, b, c});
    return func;
}

Function* LogarithmicParser::parse(const QString& input) {
    QString str = input.simplified().replace(" ", "");

    QRegularExpression re(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?log_(\d+\.?\d*|\(\d+/\d+\))\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
    QRegularExpressionMatch match = re.match(str);

    if (!match.hasMatch()) {
        re.setPattern(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?ln\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
        match = re.match(str);
        if (!match.hasMatch()) {
            re.setPattern(R"(^([+-]?(?:\d+\.?\d*|\.\d+)?)\*?log\(([+-]?(?:\d+\.?\d*|\.\d+)?)\*?x([+-]\d+\.?\d*)?\)([+-]\d+\.?\d*)?$)");
            match = re.match(str);
            if (!match.hasMatch()) {
                return nullptr;
            }
        }
    }

    double a = 1.0;
    if (match.captured(1) == "-") {
        a = -1.0;
    } else if (!match.captured(1).isEmpty() && match.captured(1) != "+") {
        a = match.captured(1).toDouble();
    }

    double base = 10.0;
    if (re.pattern().contains("log_")) {
        base = evalExpression(match.captured(2));
    } else if (re.pattern().contains("ln")) {
        base = M_E;
    }

    QString cap = match.captured(re.pattern().contains("ln") ? 2 : 3);
    double c = (cap == "-") ? -1.0 : (cap.isEmpty() ? 1.0 : cap.toDouble());
    double d = match.captured(re.pattern().contains("ln") ? 3 : 4).isEmpty() ? 0.0 : match.captured(re.pattern().contains("ln") ? 3 : 4).toDouble();
    double e = match.captured(re.pattern().contains("ln") ? 4 : 5).isEmpty() ? 0.0 : match.captured(re.pattern().contains("ln") ? 4 : 5).toDouble();

    LogarithmicFunction* func = new LogarithmicFunction();
    func->setCoefficients({a, base, c, d, e});
    return func;
}

double LogarithmicParser::evalExpression(const QString& expr) {
    if (expr.startsWith('(') && expr.endsWith(')')) {
        QString inner = expr.mid(1, expr.length() - 2);
        if (inner.contains('/')) {
            QStringList parts = inner.split('/');
            if (parts.size() == 2) {
                double num = parts[0].toDouble();
                double den = parts[1].toDouble();
                return den != 0 ? num / den : 1.0;
            }
        }
    }
    return expr.toDouble();
}

Function* ModulusParser::parse(const QString& input) {
    QString str = input.simplified().replace(" ", "");

    QRegularExpression dExpr(R"(([+-]\d+(\.\d+)?)$)");
    QRegularExpressionMatch dMatch = dExpr.match(str);

    double d = 0.0;
    if (dMatch.hasMatch()) {
        QString dStr = dMatch.captured(1);
        d = dStr.toDouble();
        str.chop(dStr.length());
    }

    double c = 1.0;
    QRegularExpression cExpr(R"(^([+-]?\d*\.?\d*)\*?\|)");
    QRegularExpressionMatch cMatch = cExpr.match(str);
    if (cMatch.hasMatch()) {
        QString cStr = cMatch.captured(1);
        if (!cStr.isEmpty() && cStr != "+" && cStr != "-")
            c = cStr.toDouble();
        else if (cStr == "-")
            c = -1.0;
        int pos = cMatch.capturedLength();
        str = str.mid(pos - 1);
    }

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
        if (str == "|x|")
            a = 1.0, b = 0.0;
        else if (str == "|-x|")
            a = -1.0, b = 0.0;
        else
            return nullptr;
    }

    ModulusFunction* func = new ModulusFunction();
    func->setCoefficients({b, d, a, c});
    return func;
}
