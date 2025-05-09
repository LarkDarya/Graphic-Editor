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
    QVector<double> coefficients;  // [d, a, b, c]

public:
    TrigonometricFunction()
        : funcType(Sin), coefficients({0.0, 1.0, 1.0, 0.0}) {}

    explicit TrigonometricFunction(Type type)
        : funcType(type), coefficients({0.0, 1.0, 1.0, 0.0}) {}

    void setType(Type type) { funcType = type; }

    Type getType() const {
        return funcType;
    }

    double evaluate(double x) const override {
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

    void setCoefficients(const QVector<double>& coeffs) override {
        coefficients = coeffs;
        while (coefficients.size() < 4) {
            coefficients.push_back(0.0);
        }
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

// Экспоненциальные функции вида a * exp(b * x)
class ExponentialFunction : public Function {
    QVector<double> coefficients; // [d, a, b, c]  - f(x) = d + a * exp(b * x + c)
public:
    ExponentialFunction() : coefficients({0.0, 1.0, 1.0, 0.0}) {}

    double evaluate(double x) const override {
        double d = coefficients.size() > 0 ? coefficients[0] : 0.0;
        double a = coefficients.size() > 1 ? coefficients[1] : 1.0;
        double b = coefficients.size() > 2 ? coefficients[2] : 1.0;
        double c = coefficients.size() > 3 ? coefficients[3] : 0.0;
        return d + a * exp(b * x + c);
    }

    void setCoefficients(const QVector<double>& coeffs) override {
        coefficients = coeffs;
        while (coefficients.size() < 4) {
            coefficients.push_back(0.0);
        }
    }

    QVector<double> getCoefficients() const override {
        return coefficients;
    }

    QString getName() const override { return "ExponentialWithOffset"; }
};


class LogarithmicFunction : public Function {
    QVector<double> coefficients; // [a, base, c, d, e]
public:
    LogarithmicFunction() : coefficients({1.0, 10.0, 1.0, 0.0, 0.0}) {}

    double evaluate(double x) const override {
        // Получаем все коэффициенты
        double a = coefficients.value(0, 1.0);
        double base = coefficients.value(1, 10.0);
        double c = coefficients.value(2, 1.0);
        double d = coefficients.value(3, 0.0);
        double e = coefficients.value(4, 0.0);

        // Вычисляем аргумент логарифма
        double arg = c * x + e;
        if (arg <= 0) return 0; // Логарифм не определен

        // Вычисляем значение
        return d + a * (log(arg) / log(base));
    }

    void setCoefficients(const QVector<double>& coeffs) override {
        coefficients = coeffs;
        // Гарантируем минимальное количество коэффициентов
        while (coefficients.size() < 5) {
            if (coefficients.size() == 2) coefficients.append(1.0); // c
            else if (coefficients.size() == 3) coefficients.append(0.0); // d
            else coefficients.append(0.0); // e
        }
    }

    QVector<double> getCoefficients() const override {
        return coefficients;
    }

    QString getName() const override {
        return "Logarithmic";
    }
};

// Модульная функции вида |x + a| + b
class ModulusFunction : public Function {
    // coefficients[0] = b (смещение внутри модуля)
    // coefficients[1] = d (смещение по y)
    // coefficients[2] = a (коэффициент при x)
    // coefficients[3] = c (множитель перед модулем)
    QVector<double> coefficients;

public:
    ModulusFunction() : coefficients({0.0, 0.0, 1.0, 1.0}) {}

    double evaluate(double x) const override {
        double b = coefficients.size() > 0 ? coefficients[0] : 0.0;
        double d = coefficients.size() > 1 ? coefficients[1] : 0.0;
        double a = coefficients.size() > 2 ? coefficients[2] : 1.0;
        double c = coefficients.size() > 3 ? coefficients[3] : 1.0;

        return d + c * std::abs(a * x + b);
    }

    void setCoefficients(const QVector<double>& coeffs) override {
        coefficients = coeffs;
        while (coefficients.size() < 4) {
            coefficients.append(0.0);
        }
    }

    QVector<double> getCoefficients() const override {
        return coefficients;
    }

    QString getName() const override {
        return "Modulus";
    }
};



#endif // FUNCTION_H
