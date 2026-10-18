#pragma once

#include <Interfaces/IInputProvider.h>

namespace inputs {

class ScopedZBack {
public:
    explicit ScopedZBack(interfaces::IInputProvider& input)
        : input(input), previous(input.isZBackEnabled()) {
        input.setZBackEnabled(true);
    }
    ~ScopedZBack() { input.setZBackEnabled(previous); }
    ScopedZBack(const ScopedZBack&) = delete;
    ScopedZBack& operator=(const ScopedZBack&) = delete;

private:
    interfaces::IInputProvider& input;
    bool previous;
};

}
