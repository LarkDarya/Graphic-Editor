#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QVector>
#include <QRegularExpression>
#include "Function.h"

class PolynomialParser {
public:
    static QVector<double> parse(const QString& input);
};

class TrigonometricParser {
public:
    static TrigonometricFunction* parse(const QString& input);
};

class ExponentialParser {
public:
    static ExponentialFunction* parse(const QString& input);
};

class LogarithmicParser {
public:
    static LogarithmicFunction* parse(const QString& input);
private:
    static double evalExpression(const QString& expr);
};

class ModulusParser {
public:
    static ModulusFunction* parse(const QString& input);
};

#endif // PARSER_H
