#include <IRremote.hpp> // defined here to avoid conflicts
#include "InfraredService.h"


namespace services {

InfraredService::InfraredService() {}

void InfraredService::begin() {
    IrSender.begin(context.getIrTxPin());
}

void InfraredService::sendRemoteCommand(RemoteCommand command) {
    int frequency = 38; // Default frequency, passed by reference to encodeRemoteCommand
    std::string protocolString = protocolRepository.getProtocolString(command.protocol);
    std::vector<float> sequence = encodeRemoteCommand(command, protocolString.c_str(), frequency);
    
    // Convert the sequence to uint16_t format as required by sendRaw
    uint16_t raw[sequence.size()];
    for (size_t i = 0; i < sequence.size(); ++i) {
        raw[i] = static_cast<uint16_t>(sequence[i]);
    }

    // Send the raw generated sequence with the correct frequency
    IrSender.sendRaw(raw, sequence.size(), frequency);
}

void InfraredService::sendFileRemoteCommand(FileRemoteCommand command) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(10, 10);
    M5Cardputer.Display.println(command.address);
    M5Cardputer.Display.println(command.function);
    M5Cardputer.Display.println(command.protocol);
    M5Cardputer.Display.println(command.frequency / 1000);
    M5Cardputer.Display.println(command.rawDataSize);

    if (command.protocol == ProtocolEnum::RAW) {
        IrSender.sendRaw(command.rawData, command.rawDataSize, command.frequency / 1000);
    } else {
        switch (command.protocol)
        {
            case ProtocolEnum::_RC5:
                IrSender.sendRC5(command.address, command.function, 0, true);
                break;
            case ProtocolEnum::_NEC:
            case ProtocolEnum::NECX2:
                IrSender.sendNEC(command.address, command.function, 0);
                break;
            default:
                break;
        }
    }
}

}