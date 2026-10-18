#ifndef ARDUINO
#include "StubInfraredSender.h"

#include <iostream>

namespace platform::linux {

void StubInfraredSender::begin(int txPin) {
    std::cerr << "[IR stub] tx pin " << txPin << " initialised\n";
}

bool StubInfraredSender::sendRaw(const uint32_t*, size_t size, int frequencyHz) {
    std::cerr << "[IR stub] raw sequence, pulses=" << size << ", frequency=" << frequencyHz << "Hz\n";
    return true;
}

}

#endif // !ARDUINO
