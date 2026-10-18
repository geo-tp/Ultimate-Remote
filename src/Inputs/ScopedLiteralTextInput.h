#pragma once

#include <Interfaces/IInputProvider.h>

namespace inputs {

class ScopedLiteralTextInput {
public:
    explicit ScopedLiteralTextInput(interfaces::IInputProvider& input)
        : input(input), previous(input.isLiteralTextInputEnabled()) {
        input.setLiteralTextInputEnabled(true);
    }
    ~ScopedLiteralTextInput() { input.setLiteralTextInputEnabled(previous); }
    ScopedLiteralTextInput(const ScopedLiteralTextInput&) = delete;
    ScopedLiteralTextInput& operator=(const ScopedLiteralTextInput&) = delete;
private:
    interfaces::IInputProvider& input;
    bool previous;
};

}
