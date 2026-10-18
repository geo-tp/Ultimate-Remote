#include "InfraredService.h"
#include <Encoders/FlipperEncoder.h>
#include <Encoders/MakeHexEncoder.h>

#include <cmath>
#include <iostream>

namespace services {

InfraredService::InfraredService(ProtocolRepository& protocolRepo, interfaces::IInfraredSender& sender)
    : protocolRepository(protocolRepo), sender(sender) {}

void InfraredService::begin() {
    sender.begin(globalContext.getIrTxPin());
}

bool InfraredService::sendRemoteCommand(RemoteCommand command, std::string manufacturerName) {
    (void)manufacturerName;
    return sendRemoteCommandChecked(command) == enums::InfraredSendResultEnum::Sent;
}


bool InfraredService::sendFileRemoteCommand(FileRemoteCommand command, std::string remoteName) {
    (void)remoteName;
    const auto wave = encoders::encodeFlipperCommand(command, fileToggle);
    if (wave.pulses.empty()) {
        std::cerr << "[IR] unsupported or invalid file command: " << command.functionName << "\n";
        return false;
    }
    const bool sent = sender.sendRawWithDuty(wave.pulses.data(), wave.pulses.size(), wave.frequencyHz, wave.dutyPercent);
    if (sent && (command.fileProtocol == "RC5" || command.fileProtocol == "RC5X" ||
                 command.fileProtocol == "RC6")) fileToggle = !fileToggle;
    return sent;
}

enums::InfraredSendResultEnum InfraredService::sendRemoteCommandChecked(const RemoteCommand& command) {
    int frequency = 38000;
    std::string protocolString = protocolRepository.getProtocolString(command.protocol);
    // IRP uses T for the alternating toggle bit on these protocol families.
    // Do not toggle arbitrary IRP definitions (e.g. Teac-K uses an 8-bit T).
    const bool usesToggle = protocolString == "rc5" || protocolString == "rc5x" ||
        protocolString == "rc6" || protocolString == "rc6-6-20" ||
        protocolString == "mce" || protocolString == "Thomson" ||
        protocolString == "recs80_45" || protocolString == "replay" ||
        protocolString == "streamzap";
    std::vector<float> sequence = encoders::encodeRemoteCommand(
        command, protocolString.c_str(), frequency, usesToggle && fileToggle);

    if (sequence.empty() || frequency < 0) {
        std::cerr << "[IR] cannot encode built-in protocol: " << protocolString << "\n";
        return enums::InfraredSendResultEnum::Unsupported;
    }

    std::vector<uint32_t> raw(sequence.size());
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (!std::isfinite(sequence[i]) || sequence[i] == 0.0f ||
            std::fabs(sequence[i]) > 10000000.0f) return enums::InfraredSendResultEnum::Unsupported;
        raw[i] = static_cast<uint32_t>(std::lround(std::fabs(sequence[i])));
    }

    if (!sender.sendRaw(raw.data(), raw.size(), frequency)) {
        std::cerr << "[IR] hardware TX failed at " << frequency
                  << " Hz for protocol " << protocolString << "\n";
        return enums::InfraredSendResultEnum::TransmitFailed;
    }
    if (usesToggle) fileToggle = !fileToggle;
    return enums::InfraredSendResultEnum::Sent;
}

}
