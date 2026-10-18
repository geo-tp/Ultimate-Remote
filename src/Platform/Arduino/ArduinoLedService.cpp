#ifdef ARDUINO
#include <Services/LedService.h>
#include <FastLED.h>

namespace services {
namespace {
constexpr int LedPin = 21;
CRGB leds[1];
bool initialized = false;
}
void LedService::showLed() {
    if (!initialized) {
        // Never register the same FastLED controller on every transmission.
        FastLED.addLeds<WS2812, LedPin, GRB>(leds, 1);
        initialized = true;
    }
    leds[0] = CRGB::OrangeRed;
    FastLED.show();
}
void LedService::clearLed() {
    if (initialized) FastLED.clear(true);
}
}
#endif
