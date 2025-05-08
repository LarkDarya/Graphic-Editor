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

// Тригонометрические функции
class TrigonometricFunction : public Function {
public:
    enum Type { Sin, Cos, Tan, Cot };
private:
    Type funcType;
    QVector<double> coefficients;

public:
    explicit TrigonometricFunction(Type type) : funcType(type), coefficients({1.0, 0.0}) {}

    double evaluate(double x) const override {
        double amplitude = coefficients.size() > 0 ? coefficients[0] : 1.0;
        double shift = coefficients.size() > 1 ? coefficients[1] : 0.0;
        double val = x + shift;

        switch(funcType) {
        case Sin: return amplitude * sin(val);
        case Cos: return amplitude * cos(val);
        case Tan: return amplitude * tan(val);
        case Cot: {
            double t = tan(val);
            return (t != 0) ? amplitude / t : 0;
        }
        }
        return 0;
    }

    void setCoefficients(const QVector<double>& coeffs) override {
        coefficients = coeffs;
    }

    QVector<double> getCoefficients() const override {
        return coefficients;
    }

    QString getName() const override {
        switch(funcType) {
        case Sin: return "Sin";
        case Cos: return "Cos";
        case Tan: return "Tan";
        case Cot: return "Cot";
        }
        return "Trigonometric";
    }
};



#endif // FUNCTION_H
