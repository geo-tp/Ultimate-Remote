#pragma once

#include <Interfaces/IInputProvider.h>

namespace inputs {

class ScopedTextInput {
public:
    explicit ScopedTextInput(interfaces::IInputProvider& input)
        : input(input), previous(input.isTextInputEnabled()) {
        input.setTextInputEnabled(true);
    }
    ~ScopedTextInput() { input.setTextInputEnabled(previous); }
    ScopedTextInput(const ScopedTextInput&) = delete;
    ScopedTextInput& operator=(const ScopedTextInput&) = delete;

private:
    interfaces::IInputProvider& input;
    bool previous;
};

}
