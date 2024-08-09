#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include <FastLED.h>
#include <Contexts/GlobalContext.h>

#define LED_PIN 21 // Builtin

using namespace contexts;


namespace services {

    class LedService {
    public:
        LedService();
        void begin();
        void blink();
        void showLed();
        void clearLed();

    private:
        GlobalContext& context = GlobalContext::getInstance();
        CRGB leds[1];
    };
}

#endif
