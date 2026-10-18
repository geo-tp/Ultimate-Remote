#ifndef STUB_INFRARED_SENDER_H
#define STUB_INFRARED_SENDER_H

#include <Interfaces/IInfraredSender.h>

namespace platform::linux {

class StubInfraredSender : public interfaces::IInfraredSender {
public:
    void begin(int txPin) override;
    bool sendRaw(const uint32_t* data, size_t size, int frequencyHz) override;
};

}

#endif
