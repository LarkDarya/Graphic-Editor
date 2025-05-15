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
    PolynomialFunction() = default; // или реализовать явно
    explicit PolynomialFunction(const QVector<double>& coeffs) : coefficients(coeffs) {}

    double evaluate(double x) const override;
    void setCoefficients(const QVector<double>& coeffs) override;
    QVector<double> getCoefficients() const override;
    QString getName() const override;
};

// Тригонометрические функции вида a * sin(b * x + c) + d
class TrigonometricFunction : public Function {
public:
    enum Type { Sin, Cos, Tan, Cot };
private:
    Type funcType;
    QVector<double> coefficients;  // [d, a, b, c]

public:
    TrigonometricFunction();
    explicit TrigonometricFunction(Type type);

    void setType(Type type);
    Type getType() const;

    double evaluate(double x) const override;
    void setCoefficients(const QVector<double>& coeffs) override;
    QVector<double> getCoefficients() const override;
    QString getName() const override;
};

// Экспоненциальные функции вида a * exp(b * x + c) + d
class ExponentialFunction : public Function {
    QVector<double> coefficients;
public:
    ExponentialFunction();

    double evaluate(double x) const override;
    void setCoefficients(const QVector<double>& coeffs) override;
    QVector<double> getCoefficients() const override;
    QString getName() const override;
};

// Логарифмические функции вида a * log_b(c * x + d) + e
class LogarithmicFunction : public Function {
    QVector<double> coefficients;
    const double NEAR_ZERO_THRESHOLD = 1e-10;
public:
    LogarithmicFunction();

    double evaluate(double x) const override;
    void setCoefficients(const QVector<double>& coeffs) override;
    QVector<double> getCoefficients() const override;
    QString getName() const override;
};

// Модульная функции вида c * |a * x + b| + d
class ModulusFunction : public Function {
    QVector<double> coefficients;
public:
    ModulusFunction();

    double evaluate(double x) const override;
    void setCoefficients(const QVector<double>& coeffs) override;
    QVector<double> getCoefficients() const override;
    QString getName() const override;
};

#endif // FUNCTION_H
