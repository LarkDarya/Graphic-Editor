#ifndef FUNCTION_H
#define FUNCTION_H

#include <QVector>
#include <QString>
#include <QtMath> // для sin, cos, tan, exp, log

// Абстрактный класс функции
class Function {
public:
    virtual ~Function() {}
    virtual double evaluate(double x) const = 0;
    virtual void setCoefficients(const QVector<double>& coeffs) = 0;
    virtual QVector<double> getCoefficients() const = 0;
    virtual QString getName() const = 0;
};

// Многочлен: a0 + a1*x + a2*x^2 + ...
class PolynomialFunction : public Function {
    QVector<double> coefficients;
public:
    double evaluate(double x) const override {
        double result = 0;
        double power = 1;
        for (double c : coefficients) {
            result += c * power;
            power *= x;
        }
        return result;
    }
    void setCoefficients(const QVector<double>& coeffs) override {
        coefficients = coeffs;
    }
    QVector<double> getCoefficients() const override {
        return coefficients;
    }
    QString getName() const override { return "Polynomial"; }
};


#endif // FUNCTION_H
