#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include <FastLED.h>
#include <Contexts/GlobalContext.h>

#define LED_PIN 21 // Builtin, TODO: use context

using namespace contexts;


namespace services {

    class LedService {
    public:
        void blink();
        void showLed();
        void clearLed();

    private:
        GlobalContext& context = GlobalContext::getInstance();
        CRGB leds[1];
    };
}

#endif
