#ifndef I_INFRARED_SENDER_H
#define I_INFRARED_SENDER_H

#include <cstddef>
#include <cstdint>

namespace interfaces {

class IInfraredSender {
public:
    virtual ~IInfraredSender() = default;

    // On Linux rc-core this pin is configured by the kernel overlay; the value is
    // kept here so the app can validate/log the expected hardware configuration.
    virtual void begin(int txPin) = 0;
    virtual bool sendRaw(const uint32_t* data, size_t size, int frequencyHz) = 0;
    virtual bool sendRawWithDuty(const uint32_t* data, size_t size, int frequencyHz, unsigned dutyPercent) {
        (void)dutyPercent;
        return sendRaw(data, size, frequencyHz);
    }
};

}

#endif
