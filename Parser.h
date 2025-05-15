#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QVector>
#include <QRegularExpression>
#include <memory>
#include "Function.h"

// Базовый класс парсера
class Parser {
public:
    virtual ~Parser() {}
    virtual Function* parse(const QString& input) = 0;
};

class PolynomialParser : public Parser {
public:
    Function* parse(const QString& input) override;
    static QVector<double> parseCoefficients(const QString& input);
};

class TrigonometricParser : public Parser {
public:
    Function* parse(const QString& input) override;
};

class ExponentialParser : public Parser {
public:
    Function* parse(const QString& input) override;
};

class LogarithmicParser : public Parser {
public:
    Function* parse(const QString& input) override;
private:
    static double evalExpression(const QString& expr);
};

class ModulusParser : public Parser {
public:
    Function* parse(const QString& input) override;
};

// Фабрика парсеров
class ParserFactory {
public:
    static std::unique_ptr<Parser> createParser(const QString& functionType) {
        if (functionType.contains("sin") || functionType.contains("cos") ||
            functionType.contains("tan") || functionType.contains("cot")) {
            return std::make_unique<TrigonometricParser>();
        } else if (functionType.contains("exp")) {
            return std::make_unique<ExponentialParser>();
        } else if (functionType.contains("log") || functionType.contains("ln")) {
            return std::make_unique<LogarithmicParser>();
        } else if (functionType.contains('|')) {
            return std::make_unique<ModulusParser>();
        } else {
            // По умолчанию пробуем парсить как полином
            return std::make_unique<PolynomialParser>();
        }
    }
};

#endif // PARSER_H
