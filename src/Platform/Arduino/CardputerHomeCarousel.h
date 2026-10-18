#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace platform::arduino {

// The Cardputer original/ADV has a 240x135 screen (vs 320x170 on Zero).
// This view deliberately lives in the ESP32 platform, not in ModeSelection.
class CardputerHomeCarousel {
public:
    static void invalidate();
    static void render(const std::vector<std::string>& labels,
                       uint16_t selected,
                       const std::vector<std::string>& descriptions);
};

} // namespace platform::arduino
