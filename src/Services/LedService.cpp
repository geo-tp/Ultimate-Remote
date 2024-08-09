#include "LedService.h"

namespace services {

LedService::LedService() {}

void LedService::begin() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, 1); // Builtin
}

void LedService::blink() {
    leds[0] = CRGB::OrangeRed;
    FastLED.show();      // led on
    delay(10);
    FastLED.clear(true); // led off
}

void LedService::showLed() {
    leds[0] = CRGB::OrangeRed;
    FastLED.show();
}

void LedService::clearLed() {
    FastLED.clear(true);
}

}