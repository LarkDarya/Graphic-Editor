#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QVector>
#include <QRegularExpression>
#include <QDebug>

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


#endif // PARSER_H
