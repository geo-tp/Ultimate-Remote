#include "FlipperEncoder.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace encoders {
// Flipper field layouts/timings: flipperdevices/flipperzero-firmware,
// lib/infrared/encoder_decoder/{nec,samsung,sirc,rc5,rc6}.
models::FlipperWaveform encodeFlipperCommand(const models::FileRemoteCommand& command, bool toggle) {
    models::FlipperWaveform wave;
    if (!command.error.empty()) return wave;
    auto& pulses = wave.pulses;
    const std::string& protocol = command.fileProtocol;
    const uint64_t address = command.address, function = command.function;
    if (protocol == "raw") {
        pulses = command.rawData;
        wave.frequencyHz = command.frequency;
        wave.dutyPercent = std::clamp(static_cast<unsigned>(std::lround(command.dutyCycle * 100)), 1u, 100u);
    } else if (protocol == "NEC" || protocol == "NECext" || protocol == "NEC42" ||
               protocol == "NEC42ext" || protocol == "Samsung32") {
        const bool samsung = protocol == "Samsung32";
        const uint32_t mark = samsung ? 550 : 560;
        uint64_t bits = 0;
        unsigned count = 32;
        if (protocol == "NEC" || samsung)
            bits = address | ((samsung ? address : (~address & 255)) << 8) |
                   (function << 16) | ((~function & 255) << 24);
        else if (protocol == "NECext") bits = address | (function << 16);
        else if (protocol == "NEC42") {
            count = 42;
            bits = address | ((~address & 8191) << 13) | (function << 26) | ((~function & 255) << 34);
        } else { count = 42; bits = address | (function << 26); }
        pulses = {samsung ? 4500u : 9000u, 4500};
        for (unsigned bit = 0; bit < count; ++bit) {
            pulses.push_back(mark);
            pulses.push_back((bits >> bit) & 1 ? (samsung ? 1650 : 1690) : mark);
        }
        pulses.push_back(mark);
    } else if (protocol == "SIRC" || protocol == "SIRC15" || protocol == "SIRC20") {
        wave.frequencyHz = 40000;
        const unsigned count = protocol == "SIRC" ? 12 : (protocol == "SIRC15" ? 15 : 20);
        const uint64_t bits = function | (address << 7);
        std::vector<uint32_t> frame {2400, 600};
        for (unsigned bit = 0; bit < count; ++bit) {
            frame.push_back((bits >> bit) & 1 ? 1200 : 600);
            if (bit + 1 < count) frame.push_back(600);
        }
        const uint32_t gap = 45000 - std::accumulate(frame.begin(), frame.end(), uint32_t(0));
        for (int repeat = 0; repeat < 3; ++repeat) {
            if (repeat) pulses.push_back(gap);
            pulses.insert(pulses.end(), frame.begin(), frame.end());
        }
    } else if (protocol == "Kaseikyo") {
        // Flipper's infrared_decoder_kaseikyo_interpret(): address packs
        // id[25:24], vendor[23:8], genre1[7:4], genre2[3:0].
        // Its 48 raw bits are sent LSB-first, including both parity fields.
        wave.frequencyHz = 38000;
        const uint16_t vendor = static_cast<uint16_t>((address >> 8) & 0xffff);
        const uint8_t p = static_cast<uint8_t>(vendor ^ (vendor >> 8));
        const uint8_t vendorParity = (p ^ (p >> 4)) & 0xf;
        const uint8_t b0 = static_cast<uint8_t>(vendor);
        const uint8_t b1 = static_cast<uint8_t>(vendor >> 8);
        const uint8_t b2 = static_cast<uint8_t>((address & 0xf0) | vendorParity);
        const uint8_t b3 = static_cast<uint8_t>((address & 0x0f) | ((function & 0xf) << 4));
        const uint8_t b4 = static_cast<uint8_t>(((function >> 4) & 0x3f) | ((address >> 18) & 0xc0));
        const uint8_t b5 = b2 ^ b3 ^ b4;
        const uint8_t bytes[6] = {b0, b1, b2, b3, b4, b5};
        pulses = {3456, 1728};
        for (uint8_t b : bytes) {
            for (unsigned bit = 0; bit < 8; ++bit) {
                pulses.push_back(432);
                pulses.push_back((b >> bit) & 1 ? 1296 : 432);
            }
        }
        pulses.push_back(432);
    } else if (protocol == "RCA") {
        wave.frequencyHz = 38000;
        const uint32_t frame = (address & 0xf) | ((function & 0xff) << 4) |
            ((~address & 0xf) << 12) | ((~function & 0xff) << 16);
        pulses = {4000, 4000};
        for (unsigned bit = 0; bit < 24; ++bit) {
            pulses.push_back(500);
            pulses.push_back(((frame >> bit) & 1u) ? 2000 : 1000);
        }
        pulses.push_back(500);
    } else if (protocol == "RC5" || protocol == "RC5X" || protocol == "RC6") {
        wave.frequencyHz = 36000;
        const bool rc6 = protocol == "RC6";
        const uint32_t unit = rc6 ? 444 : 889;
        auto level = [&](bool mark, uint32_t duration) {
            if (pulses.empty() && !mark) return;
            if (!pulses.empty() && (pulses.size() % 2 == 1) == mark) pulses.back() += duration;
            else pulses.push_back(duration);
        };
        auto bit = [&](bool value, unsigned length = 1) {
            level(rc6 ? value : !value, unit * length);
            level(rc6 ? !value : value, unit * length);
        };
        if (rc6) {
            level(true, 2666); level(false, 889);
            bit(true); bit(false); bit(false); bit(false); bit(toggle, 2);
        } else { bit(true); bit((function & 0x40) == 0); bit(toggle); }
        for (int i = rc6 ? 7 : 4; i >= 0; --i) bit((address >> i) & 1);
        for (int i = rc6 ? 7 : 5; i >= 0; --i) bit((function >> i) & 1);
    }
    if (!pulses.empty() && pulses.size() % 2 == 0) pulses.pop_back();
    return wave;
}
}
