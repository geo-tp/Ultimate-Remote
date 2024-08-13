#include <IRremote.hpp> // defined here to avoid conflicts
#include "InfraredService.h"


namespace services {

InfraredService::InfraredService() {}

void InfraredService::begin() {
    IrSender.begin(context.getIrTxPin());
}

void InfraredService::sendRemoteCommand(RemoteCommand command, std::string manufacturerName) {
    uint8_t subdevice;
    uint16_t address;
    uint16_t vendorCode;

    switch (command.protocol) {
        // Makehex seems to not handle Panasonic correctly, we use IRemote instead
        case ProtocolEnum::_PANASONIC:
        case ProtocolEnum::PANASONIC2: {
            subdevice = command.subdevice == -1 ? 0 : command.subdevice;

            // Combine device and subdevice into a 16-bit integer
            uint16_t address = (static_cast<uint16_t>(command.subdevice) << 8) | command.device;

            // Panasonic can be used by many manufacturers in the IRDB format, we check for vendor name
            vendorCode = getKaseikyoVendorIdCode(manufacturerName);
            
            IrSender.sendKaseikyo(address, command.function, 0, vendorCode);
            break;
        }
        // Same for Sony20
        case ProtocolEnum::SONY20: {
            subdevice = command.subdevice == -1 ? 0 : command.subdevice;

            // Combine device and subdevice into a 16-bit integer
            uint16_t address = (static_cast<uint16_t>(command.subdevice) << 8) | command.device;

            IrSender.sendSony(address, command.function, 0, SIRCS_20_PROTOCOL);  
            break;
        }
        // Handle by MakeHex
        default: {
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
            break;
        }
    }
}


void InfraredService::sendFileRemoteCommand(FileRemoteCommand command, std::string remoteName) {
    uint16_t vendorCode;

    if (command.protocol == ProtocolEnum::RAW) {
        IrSender.sendRaw(command.rawData, command.rawDataSize, command.frequency);
    } else {

        switch (command.protocol) {
            case ProtocolEnum::_RC5:
            case ProtocolEnum::RC5X:
                IrSender.sendRC5(command.address, command.function, 0, true);
                break;
            case ProtocolEnum::_RC6:
                IrSender.sendRC6(command.address, command.function, 0, true);
                break;
            case ProtocolEnum::_NEC:
            case ProtocolEnum::NECX2:
            case ProtocolEnum::NEC42:
            case ProtocolEnum::NEC42ext:
                IrSender.sendNEC(command.address, command.function, 0);
                break;
            case ProtocolEnum::SIRC:
                IrSender.sendSony(command.address, command.function, 0, SIRCS_12_PROTOCOL);
                break;
            case ProtocolEnum::SIRC15:
                IrSender.sendSony(command.address, command.function, 0, SIRCS_15_PROTOCOL);
                break;
            case ProtocolEnum::SIRC20:
                IrSender.sendSony(command.address, command.function, 0, SIRCS_20_PROTOCOL);  
                break;
            case ProtocolEnum::SAMSUNG32:
                IrSender.sendSamsung(command.address, command.function, 0);  
                break;
            case ProtocolEnum::_KASEIKYO:
                vendorCode = getKaseikyoVendorIdCode(remoteName);
                IrSender.sendKaseikyo(command.address, command.function, 0, vendorCode);  
                break;
            default:
                IrSender.sendNEC2(command.address, command.function, 0); // NEC is the most commun, NEC2 for reliablity
        }
    }
}

uint16_t InfraredService::getKaseikyoVendorIdCode(const std::string& input) {
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

    if (lowerInput.find("panasonic") != std::string::npos) {
        return PANASONIC_VENDOR_ID_CODE;
    } else if (lowerInput.find("denon") != std::string::npos) {
        return DENON_VENDOR_ID_CODE;
    } else if (lowerInput.find("mitsubishi") != std::string::npos) {
        return MITSUBISHI_VENDOR_ID_CODE;
    } else if (lowerInput.find("sharp") != std::string::npos) {
        return SHARP_VENDOR_ID_CODE;
    } else if (lowerInput.find("jvc") != std::string::npos) {
        return JVC_VENDOR_ID_CODE;
    } else {
        return PANASONIC_VENDOR_ID_CODE; // default
    }
}

}