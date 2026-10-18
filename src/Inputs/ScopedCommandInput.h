#pragma once

#include <Interfaces/IInputProvider.h>

namespace inputs {

class ScopedCommandInput {
public:
    explicit ScopedCommandInput(interfaces::IInputProvider& input)
        : input(input), previous(input.isCommandInputEnabled()) { input.setCommandInputEnabled(true); }
    ~ScopedCommandInput() { input.setCommandInputEnabled(previous); }
    ScopedCommandInput(const ScopedCommandInput&) = delete;
    ScopedCommandInput& operator=(const ScopedCommandInput&) = delete;
private:
    interfaces::IInputProvider& input;
    bool previous;
};

}
