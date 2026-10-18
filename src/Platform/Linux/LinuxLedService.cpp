#ifndef ARDUINO
#include <Services/LedService.h>
#include <iostream>
namespace services {
void LedService::showLed() { std::cerr << "[LED stub] on\n"; }
void LedService::clearLed() { std::cerr << "[LED stub] off\n"; }
}
#endif
