#ifndef ARDUINO
#include "KernelLircInfraredSender.h"

#include <cerrno>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/lirc.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace platform::linux {

KernelLircInfraredSender::KernelLircInfraredSender(std::string devicePath)
    : devicePath(std::move(devicePath)) {}

KernelLircInfraredSender::~KernelLircInfraredSender() {
    if (fd >= 0) {
        close(fd);
    }
}

void KernelLircInfraredSender::begin(int txPin) {
    configuredTxPin = txPin;
    if (!ensureOpen()) {
        return;
    }

    queryFeatures();
    if (!canSendPulse) {
        discoverTxDevice();
    }

    if (!canSendPulse) {
        std::cerr << "[IR lirc] opened " << devicePath
                  << " but no TX-capable /dev/lircX was found";
    } else {
        std::cerr << "[IR lirc] using kernel rc-core device " << devicePath << " for TX";
    }
    if (configuredTxPin >= 0) {
        std::cerr << " (expected gpio-ir-tx/pwm-ir-tx pin " << configuredTxPin << ")";
    }
    std::cerr << "\n";
}

bool KernelLircInfraredSender::sendRawWithDuty(const uint32_t* data, size_t size, int frequencyHz, unsigned duty) {
    dutyPercent = std::clamp(duty, 1u, 100u);
    const bool sent = sendRaw(data, size, frequencyHz);
    dutyPercent = 33;
    return sent;
}

bool KernelLircInfraredSender::sendRaw(const uint32_t* data, size_t size, int frequencyHz) {
    if (!data || size == 0 || !ensureOpen()) {
        return false;
    }

    queryFeatures();
    if (!canSendPulse) {
        discoverTxDevice();
    }

    if (!canSendPulse) {
        std::cerr << "[IR lirc] refusing raw TX: " << devicePath
                  << " does not report LIRC_CAN_SEND_PULSE. This is likely an RX device; "
                  << "try another /dev/lircX or enable dtoverlay=gpio-ir-tx/pwm-ir-tx.\n";
        return false;
    }

    // A carrier-capable LIRC driver would keep modulating the previous carrier
    // if passed 0 Hz. Only unmodulated gpio-ir-tx-style devices can safely
    // represent zero-carrier protocols (Barco/Jerrold).
    if (frequencyHz == 0 && carrierConfigSupported) {
        std::cerr << "[IR lirc] zero-carrier waveform requires an unmodulated TX device\n";
        return false;
    }
    if (frequencyHz < 0) return false;
    configureCarrier(frequencyHz);

    std::vector<lirc_t> pulses;
    pulses.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        if (data[i] > 0) {
            pulses.push_back(static_cast<lirc_t>(data[i]));
        }
    }

    // rc-core LIRC pulse mode expects pulse/space durations starting and ending
    // with a pulse. That means an odd number of samples.
    if (pulses.size() > 1 && pulses.size() % 2 == 0) {
        pulses.pop_back();
    }

    if (pulses.empty()) {
        std::cerr << "[IR lirc] empty raw sequence ignored\n";
        return false;
    }

    ssize_t expected = static_cast<ssize_t>(pulses.size() * sizeof(unsigned int));
    ssize_t written = write(fd, pulses.data(), static_cast<size_t>(expected));
    if (written != expected) {
        std::cerr << "[IR lirc] write failed on " << devicePath
                  << ": " << std::strerror(errno)
                  << " (samples=" << pulses.size()
                  << ", bytes=" << expected
                  << ", carrier=" << frequencyHz << "Hz)\n";
        return false;
    }

    if (const char* debug = std::getenv("ULTIMATE_REMOTE_IR_DEBUG"); debug && debug[0] != '\0' && debug[0] != '0') {
        std::cerr << "[IR lirc] TX ok on " << devicePath
                  << " samples=" << pulses.size()
                  << " bytes=" << expected
                  << " carrier=" << frequencyHz << "Hz\n";
    }

    return true;
}

bool KernelLircInfraredSender::ensureOpen() {
    if (fd >= 0) {
        return true;
    }

    fd = open(devicePath.c_str(), O_RDWR);
    if (fd < 0) {
        std::cerr << "[IR lirc] cannot open " << devicePath << ": " << std::strerror(errno) << "\n";
        std::cerr << "[IR lirc] enable TX with dtoverlay=gpio-ir-tx,gpio_pin="
                  << (configuredTxPin >= 0 ? configuredTxPin : 12)
                  << " or dtoverlay=pwm-ir-tx,gpio_pin="
                  << (configuredTxPin >= 0 ? configuredTxPin : 12)
                  << ", then verify with ir-ctl -d " << devicePath << " --features\n";
        return false;
    }

#ifdef LIRC_SET_SEND_MODE
    int mode = LIRC_MODE_PULSE;
    if (ioctl(fd, LIRC_SET_SEND_MODE, &mode) < 0 && errno != ENOTTY) {
        std::cerr << "[IR lirc] cannot set pulse send mode: " << std::strerror(errno) << "\n";
    }
#endif

    return true;
}

void KernelLircInfraredSender::queryFeatures() {
    if (featuresQueried || fd < 0) {
        return;
    }

    featuresQueried = true;

#ifdef LIRC_GET_FEATURES
    __u32 features = 0;
    if (ioctl(fd, LIRC_GET_FEATURES, &features) == 0) {
        canSendPulse = (features & LIRC_CAN_SEND_PULSE) != 0;
        carrierConfigSupported = (features & LIRC_CAN_SET_SEND_CARRIER) != 0;
        std::cerr << "[IR lirc] features=0x" << std::hex << features << std::dec << "\n";
        if (!canSendPulse) {
            std::cerr << "[IR lirc] warning: device does not report LIRC_CAN_SEND_PULSE\n";
        }
        return;
    }
#endif

    canSendPulse = true;
}

bool KernelLircInfraredSender::discoverTxDevice() {
#ifndef LIRC_GET_FEATURES
    return false;
#else
    for (int i = 0; i < 16; ++i) {
        std::string candidate = "/dev/lirc" + std::to_string(i);
        if (candidate == devicePath) {
            continue;
        }

        int candidateFd = open(candidate.c_str(), O_RDWR);
        if (candidateFd < 0) {
            continue;
        }

        __u32 features = 0;
        bool candidateCanSend = ioctl(candidateFd, LIRC_GET_FEATURES, &features) == 0
            && (features & LIRC_CAN_SEND_PULSE) != 0;
        if (!candidateCanSend) {
            close(candidateFd);
            continue;
        }

        if (fd >= 0) {
            close(fd);
        }

        fd = candidateFd;
        devicePath = candidate;
        featuresQueried = true;
        canSendPulse = true;
        carrierConfigSupported = (features & LIRC_CAN_SET_SEND_CARRIER) != 0;

#ifdef LIRC_SET_SEND_MODE
        int mode = LIRC_MODE_PULSE;
        if (ioctl(fd, LIRC_SET_SEND_MODE, &mode) < 0 && errno != ENOTTY) {
            std::cerr << "[IR lirc] cannot set pulse send mode on " << devicePath
                      << ": " << std::strerror(errno) << "\n";
        }
#endif

        std::cerr << "[IR lirc] auto-selected TX device " << devicePath
                  << " features=0x" << std::hex << features << std::dec << "\n";
        return true;
    }

    return false;
#endif
}

void KernelLircInfraredSender::configureCarrier(int frequencyHz) {
    if (fd < 0 || frequencyHz <= 0) {
        return;
    }

#ifdef LIRC_SET_SEND_CARRIER
    unsigned int frequency = static_cast<unsigned int>(frequencyHz);
    if (ioctl(fd, LIRC_SET_SEND_CARRIER, &frequency) < 0 && errno != ENOTTY) {
        std::cerr << "[IR lirc] cannot set carrier " << frequency << "Hz: "
                  << std::strerror(errno) << "\n";
    }
#endif

#ifdef LIRC_SET_SEND_DUTY_CYCLE
    unsigned int dutyCycle = dutyPercent;
    if (ioctl(fd, LIRC_SET_SEND_DUTY_CYCLE, &dutyCycle) < 0 && errno != ENOTTY) {
        std::cerr << "[IR lirc] cannot set duty cycle: " << std::strerror(errno) << "\n";
    }
#endif
}

}

#endif // !ARDUINO
