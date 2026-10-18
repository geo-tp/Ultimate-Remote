#ifdef ARDUINO

#include "ArduinoInfraredSender.h"

#include <Arduino.h>
#include <cstdint>

namespace platform::arduino {
namespace {
// On Arduino-ESP32 3.x, LEDC can generate the exact carrier and duty cycle.
// Avoid Arduino-IRremote's uint16_t raw timings: its 65,535us limit silently
// shortened internal spaces in several MakeHex protocols and Flipper captures.
// The ESP32-S3 Arduino core normally clocks LEDC from a 40 MHz XTAL.
// 40 kHz * 2^10 = 40.96 MHz: Sony's carrier CANNOT attach at 10 bits!
// 56/58 kHz (Zaptor/RCA) fail too. 8 bits work through 120 kHz;
// use 7 for faster experimental carriers (up to 200 kHz).
constexpr uint8_t carrierResolution(uint32_t frequencyHz) {
    return frequencyHz > 120000u ? 7u : 8u;
}
constexpr uint32_t kMaxDurationUs = 10000000;

void waitMicros(uint32_t us) {
    if (us >= 20000) {
        // Yield while waiting long spaces, without losing the fractional part.
        delay(us / 1000);
        us %= 1000;
    }
    if (us) delayMicroseconds(us);
}
} // namespace

void ArduinoInfraredSender::begin(int txPin) {
    pin = txPin;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

bool ArduinoInfraredSender::sendRaw(const uint32_t* data, size_t size, int frequencyHz) {
    return sendRawWithDuty(data, size, frequencyHz, 33);
}

bool ArduinoInfraredSender::sendRawWithDuty(
    const uint32_t* data, size_t size, int frequencyHz, unsigned dutyPercent) {
    if (!data || size == 0 || size > 65535 || pin < 0 || dutyPercent == 0 || dutyPercent > 100 ||
        (frequencyHz != 0 && (frequencyHz < 1000 || frequencyHz > 200000))) return false;
    // Validate *before* transmitting: no partial waveforms on malformed input.
    for (size_t i = 0; i < size; ++i) {
        if (data[i] == 0 || data[i] > kMaxDurationUs) return false;
        // Reject excessively long active marks, especially 100% duty or
        // unmodulated GPIO HIGH, to protect the IR LED and its driver.
        if ((i % 2u) == 0u &&
            data[i] > ((frequencyHz == 0 || dutyPercent == 100u) ? 30000u : 250000u)) return false;
    }

    const bool modulated = frequencyHz > 0;
    if (modulated) {
        const uint8_t resolution = carrierResolution(static_cast<uint32_t>(frequencyHz));
        if (!ledcAttach(pin, static_cast<uint32_t>(frequencyHz), resolution)) return false;
        if (!ledcWrite(pin, 0)) {
            ledcDetach(pin);
            return false;
        }
    } else {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
    const uint8_t resolution = carrierResolution(static_cast<uint32_t>(frequencyHz));
    const uint32_t duty = (static_cast<uint32_t>(dutyPercent) * ((1u << resolution) - 1u) + 50u) / 100u;
    bool sent = true;
    for (size_t i = 0; i < size; ++i) {
        const bool mark = (i % 2) == 0;
        if (modulated) {
            if (!ledcWrite(pin, mark ? duty : 0)) {
                sent = false;
                break;
            }
        } else digitalWrite(pin, mark ? HIGH : LOW);
        waitMicros(data[i]);
    }
    if (modulated) {
        if (!ledcWrite(pin, 0)) sent = false;
        if (!ledcDetach(pin)) sent = false;
    }
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    return sent;
}

} // namespace platform::arduino
#endif
