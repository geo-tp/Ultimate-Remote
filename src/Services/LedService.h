#ifndef LED_SERVICE_H
#define LED_SERVICE_H

namespace services {
class LedService {
public:
    void blink();
    void showLed();
    void clearLed();
};
}
#endif
