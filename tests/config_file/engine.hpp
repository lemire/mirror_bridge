// Test config-file with multiple header files
#include <string>

struct Engine {
    std::string type;
    int horsepower;

    Engine() : horsepower(0) {}
    Engine(std::string t, int hp) : type(t), horsepower(hp) {}

    bool is_powerful() const {
        return horsepower > 300;
    }
};
