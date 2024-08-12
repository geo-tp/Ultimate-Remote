#include "FileService.h"

namespace services {

ProtocolRepository FileService::protocolRepository;

FileRemote FileService::getRemoteFromFile(const std::string& fileName, const std::string& fileContent) {
    std::vector<FileRemoteCommand> commands = parseInfraredFile(fileContent);
    FileRemote remote = {fileName, commands};

    return remote;
}

bool FileService::validateInfraredFile(const std::string& fileContent) {
    std::string line;

    if (fileContent == "") {
        return false;
    }

    // Split the fileContent into lines
    std::istringstream stream(fileContent);
    size_t i = 1;
    while (std::getline(stream, line)) {
        if (line.find("Filetype:") != 0 && i == 1) {
            return false;
        }
        if (line.find("Version:") != 0 && i == 2) {
            return false;
        }
        i++;
    }

    return true;
}

std::vector<FileRemoteCommand> FileService::parseInfraredFile(const std::string& fileContent) {
    std::vector<FileRemoteCommand> commands;
    FileRemoteCommand command;
    std::vector<std::string> lines;
    std::string value;
    std::string line;
    int count = 0;

    // Split the fileContent into lines
    std::istringstream stream(fileContent);
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    // Iterate over each line
    for (const auto& line : lines) {
        value = line.substr(line.find(":") + 1);
        value = StringUtils::trim(value);
        
        if (line.find("name:") == 0) {

            // We push the existing command if any
            if (!command.functionName.empty()) {
                commands.push_back(command);
            }

            // We reset command for the new one
            command = FileRemoteCommand();
            command.functionName = value;
        }
        if (line.find("type:") == 0 && value == "raw") {
            command.protocol = protocolRepository.getProtocolEnum("raw");
        }
        if (line.find("protocol:") == 0) {
            command.protocol = protocolRepository.getProtocolEnum(value);
        }
        if (line.find("address:") == 0) {
            command.address = StringUtils::convertHexToUint16(value);
        }
        if (line.find("frequency:") == 0) {
            command.frequency = std::stoi(value);
        }
        if (line.find("command:") == 0) {
            command.function = StringUtils::convertHexToUint16(value, 1);
        }
        if (line.find("duty_cycle:") == 0) {
            command.dutyCycle = std::stof(value);
        }
        if (line.find("data:") == 0) {
            size_t size;
            uint16_t* rawData = StringUtils::convertDecToUint16Array(value, size);
            command.rawData = rawData;
            command.rawDataSize = size;
        }

    }

    // Don't forget to push the last command to the vector
    if (!command.functionName.empty()) {
        commands.push_back(command);
    }

    return commands;
}

}
