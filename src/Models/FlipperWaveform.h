#pragma once

#include <cstdint>
#include <vector>

namespace models {
struct FlipperWaveform {
    std::vector<uint32_t> pulses;
    int frequencyHz = 38000;
    unsigned dutyPercent = 33;
};
}
