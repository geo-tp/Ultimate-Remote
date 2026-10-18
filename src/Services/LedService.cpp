#include "LedService.h"
#include <Platform/Compatibility/ArduinoCompat.h>
namespace services {
void LedService::blink() {
    showLed();
    delay(10);
    clearLed();
}
}
