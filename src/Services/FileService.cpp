#include "FileService.h"

namespace services {

std::vector<FileRemoteCommand> FileService::parseIrFile(const std::string& fileContent) {
    std::vector<FileRemoteCommand> commands;
    FileRemoteCommand command;
    std::vector<std::string> lines;
    std::string value;
    int count = 0;

    // Split the fileContent into lines
    std::string line;
    std::istringstream stream(fileContent);
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    // Iterate over each line
    for (const auto& line : lines) {
        FileRemoteCommand command;
        std::string value = line.substr(line.find(":") + 1);
        value = StringUtils::trim(value);

        if (line.find("name:") == 0) {
            command.commandName = value.erase(0, 5);
        }
        if (line.find("type:") && line.find("raw") == 0) {
            command.protocol = protocolRepository.getProtocolEnum("raw");
        }
        if (line.find("protocol:") == 0) {
            command.protocol = protocolRepository.getProtocolEnum(value.erase(0, 9));
        }
        if (line.find("address:") == 0) {
            command.address = StringUtils::convertToUint16(value.erase(0, 8));
        }
        if (line.find("frequency:") == 0) {
            command.frequency = std::stoi(value.erase(0, 10));
        }
        if (line.find("command:") == 0) {
            std::istringstream iss(value.erase(0, 8));
            std::string byteStr;
            // Lire le premier groupe d'octets (avant le premier espace)
            iss >> byteStr;
            uint8_t value = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
            command.command = value;

        }
        if (line.find("duty_cycle:") == 0) {
            command.dutyCycle = std::stof(value.erase(0, 8));
        }
        if (line.find("data:") == 0) {
            size_t size;
            uint16_t* rawData = StringUtils::convertToUint16Array(value.erase(0, 5), size);
            command.rawData = rawData;
            command.rawDataSize = size;
        }

        if (!command.commandName.empty()) {
            commands.push_back(command);
        }
    }

    return commands;
}

}
