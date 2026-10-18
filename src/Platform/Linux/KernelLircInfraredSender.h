#ifndef KERNEL_LIRC_INFRARED_SENDER_H
#define KERNEL_LIRC_INFRARED_SENDER_H

#include <Interfaces/IInfraredSender.h>

#include <string>

namespace platform::linux {

class KernelLircInfraredSender : public interfaces::IInfraredSender {
public:
    explicit KernelLircInfraredSender(std::string devicePath);
    ~KernelLircInfraredSender() override;

    void begin(int txPin) override;
    bool sendRaw(const uint32_t* data, size_t size, int frequencyHz) override;
    bool sendRawWithDuty(const uint32_t* data, size_t size, int frequencyHz, unsigned dutyPercent) override;

private:
    std::string devicePath;
    int fd = -1;
    int configuredTxPin = -1;
    bool canSendPulse = false;
    bool carrierConfigSupported = false;
    bool featuresQueried = false;
    unsigned dutyPercent = 33;

    bool ensureOpen();
    void queryFeatures();
    bool discoverTxDevice();
    void configureCarrier(int frequencyHz);
};

}

#endif
