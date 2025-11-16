// Example from README introduction
struct Calculator {
    double value = 0.0;

    Calculator() = default;
    Calculator(double initial) : value(initial) {}

    double add(double x) { return value += x; }
    double subtract(double x) { return value -= x; }
};
