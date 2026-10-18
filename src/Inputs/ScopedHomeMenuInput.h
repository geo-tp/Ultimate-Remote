#pragma once

#include <Interfaces/IInputProvider.h>

namespace inputs {

class ScopedHomeMenuInput {
public:
    explicit ScopedHomeMenuInput(interfaces::IInputProvider& input) : input(input) { input.setHomeMenuActive(true); }
    ~ScopedHomeMenuInput() { input.setHomeMenuActive(false); }
    ScopedHomeMenuInput(const ScopedHomeMenuInput&) = delete;
    ScopedHomeMenuInput& operator=(const ScopedHomeMenuInput&) = delete;
private:
    interfaces::IInputProvider& input;
};

}
