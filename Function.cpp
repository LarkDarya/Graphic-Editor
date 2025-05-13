#include "Function.h"

// Многочлен: a0 + a1*x + a2*x^2 + ...
double PolynomialFunction::evaluate(double x) const {
    double result = 0;
    double power = 1;
    for (double c : coefficients) {
        result += c * power;
        power *= x;
    }
    return result;
}

void PolynomialFunction::setCoefficients(const QVector<double>& coeffs) {
    coefficients = coeffs;
}

QVector<double> PolynomialFunction::getCoefficients() const {
    return coefficients;
}

QString PolynomialFunction::getName() const {
    return "Polynomial";
}

// Тригонометрические функции вида a * sin(b * x + c) + d
TrigonometricFunction::TrigonometricFunction()
    : funcType(Sin), coefficients({0.0, 1.0, 1.0, 0.0}) {}

TrigonometricFunction::TrigonometricFunction(Type type)
    : funcType(type), coefficients({0.0, 1.0, 1.0, 0.0}) {}

void TrigonometricFunction::setType(Type type) { funcType = type; }

TrigonometricFunction::Type TrigonometricFunction::getType() const {
    return funcType;
}

double TrigonometricFunction::evaluate(double x) const {
    double d = coefficients.size() > 0 ? coefficients[0] : 0.0;
    double a = coefficients.size() > 1 ? coefficients[1] : 1.0;
    double b = coefficients.size() > 2 ? coefficients[2] : 1.0;
    double c = coefficients.size() > 3 ? coefficients[3] : 0.0;

    double val = b * x + c;

    switch (funcType) {
    case Sin: return d + a * sin(val);
    case Cos: return d + a * cos(val);
    case Tan: return d + a * tan(val);
    case Cot: {
        double t = tan(val);
        return d + (t != 0 ? a / t : 0);
    }
    }
    return 0;
}

void TrigonometricFunction::setCoefficients(const QVector<double>& coeffs) {
    coefficients = coeffs;
    while (coefficients.size() < 4) {
        coefficients.push_back(0.0);
    }
}

QVector<double> TrigonometricFunction::getCoefficients() const {
    return coefficients;
}

QString TrigonometricFunction::getName() const {
    switch(funcType) {
    case Sin: return "Sin";
    case Cos: return "Cos";
    case Tan: return "Tan";
    case Cot: return "Cot";
    }
    return "Trigonometric";
}

// Экспоненциальные функции вида a * exp(b * x + c) + d
ExponentialFunction::ExponentialFunction() : coefficients({0.0, 1.0, 1.0, 0.0}) {}

double ExponentialFunction::evaluate(double x) const {
    double d = coefficients.size() > 0 ? coefficients[0] : 0.0;
    double a = coefficients.size() > 1 ? coefficients[1] : 1.0;
    double b = coefficients.size() > 2 ? coefficients[2] : 1.0;
    double c = coefficients.size() > 3 ? coefficients[3] : 0.0;
    return d + a * exp(b * x + c);
}

void ExponentialFunction::setCoefficients(const QVector<double>& coeffs) {
    coefficients = coeffs;
    while (coefficients.size() < 4) {
        coefficients.push_back(0.0);
    }
}

QVector<double> ExponentialFunction::getCoefficients() const {
    return coefficients;
}

QString ExponentialFunction::getName() const {
    return "ExponentialWithOffset";
}

// Логарифмические функции вида a * log_b(c * x + d) + e
LogarithmicFunction::LogarithmicFunction() : coefficients({1.0, 10.0, 1.0, 0.0, 0.0}) {}

double LogarithmicFunction::evaluate(double x) const {
    double a = coefficients.value(0, 1.0);
    double base = coefficients.value(1, 10.0);
    double c = coefficients.value(2, 1.0);
    double d = coefficients.value(3, 0.0);
    double e = coefficients.value(4, 0.0);

    double arg = c * x + d;

    if (arg <= 0.0) {
        return (a > 0) ? -1e10 : 1e10;
    }

    if (arg < NEAR_ZERO_THRESHOLD) {
        return (a > 0) ? -1e10 : 1e10;
    }

    return e + a * (std::log(arg) / std::log(base));
}

void LogarithmicFunction::setCoefficients(const QVector<double>& coeffs) {
    coefficients = coeffs;
    while (coefficients.size() < 5) {
        if (coefficients.size() == 2) coefficients.append(1.0);
        else if (coefficients.size() == 3) coefficients.append(0.0);
        else coefficients.append(0.0);
    }
}

QVector<double> LogarithmicFunction::getCoefficients() const {
    return coefficients;
}

QString LogarithmicFunction::getName() const {
    return "Logarithmic";
}

// Модульная функции вида c * |a * x + b| + d
ModulusFunction::ModulusFunction() : coefficients({0.0, 0.0, 1.0, 1.0}) {}

double ModulusFunction::evaluate(double x) const {
    double b = coefficients.size() > 0 ? coefficients[0] : 0.0;
    double d = coefficients.size() > 1 ? coefficients[1] : 0.0;
    double a = coefficients.size() > 2 ? coefficients[2] : 1.0;
    double c = coefficients.size() > 3 ? coefficients[3] : 1.0;

    return d + c * std::abs(a * x + b);
}

void ModulusFunction::setCoefficients(const QVector<double>& coeffs) {
    coefficients = coeffs;
    while (coefficients.size() < 4) {
        coefficients.append(0.0);
    }
}

QVector<double> ModulusFunction::getCoefficients() const {
    return coefficients;
}

QString ModulusFunction::getName() const {
    return "Modulus";
}
