#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

#ifdef ARDUINO

#include <Arduino.h>

#else

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

#ifndef PROGMEM
#define PROGMEM
#endif

inline void delay(unsigned long milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

inline unsigned long millis() {
    static const auto start = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();
    return static_cast<unsigned long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());
}

using String = std::string;

#endif

#endif
