#ifndef ARDUINO_INFRARED_SENDER_H
#define ARDUINO_INFRARED_SENDER_H

#include <Interfaces/IInfraredSender.h>

namespace platform::arduino {

class ArduinoInfraredSender : public interfaces::IInfraredSender {
public:
    void begin(int txPin) override;
    bool sendRaw(const uint32_t* data, size_t size, int frequencyHz) override;
    bool sendRawWithDuty(const uint32_t* data, size_t size, int frequencyHz, unsigned dutyPercent) override;
private:
    int pin = -1;
};

}

#endif
