#pragma once
#include <string>
#include <vector>

// Test class for single-header verification
struct Robot {
    std::string name;
    double battery = 100.0;
    std::vector<std::string> commands;

    // Method: move forward
    void move_forward(double distance) {
        battery -= distance * 0.1;
    }

    // Method: get status
    std::string get_status() const {
        return name + " (battery: " + std::to_string(battery) + "%)";
    }

    // Method: add command
    void add_command(const std::string& cmd) {
        commands.push_back(cmd);
    }

    // Method: command count
    int command_count() const {
        return commands.size();
    }

    // Method: reset
    void reset() {
        battery = 100.0;
        commands.clear();
    }
};
