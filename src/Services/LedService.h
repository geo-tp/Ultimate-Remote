#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include <FastLED.h>
#include <Contexts/GlobalContext.h>

#define LED_PIN 21 // Builtin, TODO: use context

namespace services {

    class LedService {
    public:
        void blink();
        void showLed();
        void clearLed();

    private:
        contexts::GlobalContext& context = contexts::GlobalContext::getInstance();
        CRGB leds[1];
    };
}

#endif
