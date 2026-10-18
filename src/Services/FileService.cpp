#include "FileService.h"
#include <cmath>
#include <map>
#include <stdexcept>

namespace services {
namespace {
std::string trim(const std::string& text) {
    const auto begin = text.find_first_not_of(" \t\r\n");
    return begin == std::string::npos ? "" : text.substr(begin, text.find_last_not_of(" \t\r\n") - begin + 1);
}
uint32_t number(const std::string& text, uint32_t maximum, int base = 10) {
    if (text.empty() || text[0] == '-' || text[0] == '+') throw std::invalid_argument("number");
    size_t end;
    const auto value = std::stoul(text, &end, base);
    if (end != text.size() || value > maximum) throw std::invalid_argument("number");
    return static_cast<uint32_t>(value);
}
uint32_t flipperHex(const std::string& text) {
    std::istringstream stream(text);
    std::string byte;
    uint32_t value = 0;
    int shift = 0;
    while (stream >> byte) {
        if (shift >= 32 || byte.size() != 2) throw std::invalid_argument("hex");
        value |= number(byte, 255, 16) << shift;
        shift += 8;
    }
    if (shift != 32) throw std::invalid_argument("hex");
    return value;
}
}
FileService::FileService(ProtocolRepository& protocolRepo) : protocolRepository(protocolRepo) {}

FileRemote FileService::getRemoteFromFile(const std::string& fileName, const std::string& content) {
    if (!validateInfraredFile(content)) return {fileName, {}};
    return {fileName, parseInfraredFile(content)};
}

bool FileService::validateInfraredFile(const std::string& content) {
    if (content.empty() || content.size() > 4 * 1024 * 1024 || content.find('\0') != std::string::npos) return false;
    std::istringstream stream(content.compare(0, 3, "\xef\xbb\xbf") == 0 ? content.substr(3) : content);
    std::string line;
    int header = 0;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        if (header == 0 && line != "Filetype: IR signals file" &&
            line != "Filetype: IR library file") return false;
        if (header == 1) return line == "Version: 1";
        ++header;
    }
    return false;
}

std::vector<FileRemoteCommand> FileService::parseInfraredFile(const std::string& content) {
    std::vector<FileRemoteCommand> commands;
    std::map<std::string, std::string> fields;
    auto finish = [&]() {
        if (!fields.count("name") || fields["name"].empty() || commands.size() >= 2048) return;
        FileRemoteCommand command;
        command.functionName = fields["name"].substr(0, 128);
        try {
            if (fields["type"] == "raw") {
                command.protocol = ProtocolEnum::RAW;
                command.fileProtocol = "raw";
                const auto hz = number(fields.at("frequency"), 1000000);
                if (hz < 10000) throw std::invalid_argument("frequency");
                command.frequency = static_cast<int>(hz);
                size_t end;
                command.dutyCycle = std::stof(fields.at("duty_cycle"), &end);
                if (end != fields["duty_cycle"].size() || !std::isfinite(command.dutyCycle) ||
                    command.dutyCycle <= 0 || command.dutyCycle > 1) throw std::invalid_argument("duty");
                std::istringstream data(fields.at("data"));
                std::string duration;
                uint64_t total = 0;
                while (data >> duration) {
                    const auto value = number(duration, 1000000);
                    total += value;
                    if (!value || command.rawData.size() >= 65535 || total > 10000000) throw std::invalid_argument("data");
                    command.rawData.push_back(value);
                }
                if (command.rawData.empty()) throw std::invalid_argument("data");
            } else if (fields["type"] == "parsed") {
                command.fileProtocol = fields.at("protocol");
                command.protocol = protocolRepository.getProtocolEnum(command.fileProtocol);
                command.address = flipperHex(fields.at("address"));
                command.function = flipperHex(fields.at("command"));
                const std::map<std::string, std::pair<uint32_t, uint32_t>> limits = {
                    {"NEC", {255,255}}, {"NECext", {65535,65535}}, {"Samsung32", {255,255}},
                    {"NEC42", {8191,255}}, {"NEC42ext", {0x3ffffff,65535}},
                    {"SIRC", {31,127}}, {"SIRC15", {255,127}}, {"SIRC20", {8191,127}},
                    {"RC5", {31,63}}, {"RC5X", {31,127}}, {"RC6", {255,255}},
                    {"Kaseikyo", {0x3ffffff, 1023}}, {"RCA", {15,255}}
                };
                const auto found = limits.find(command.fileProtocol);
                if (found == limits.end()) command.error = "Unsupported protocol: " + command.fileProtocol;
                else if (command.address > found->second.first || command.function > found->second.second)
                    throw std::invalid_argument("range");
            } else throw std::invalid_argument("type");
        } catch (const std::exception&) {
            command.error = "Invalid IR data for this command";
            command.rawData.clear();
        }
        commands.push_back(std::move(command));
    };
    std::istringstream stream(content.compare(0, 3, "\xef\xbb\xbf") == 0 ? content.substr(3) : content);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        const std::string key = trim(line.substr(0, colon));
        if (key == "name") { finish(); fields.clear(); }
        fields[key] = trim(line.substr(colon + 1));
    }
    finish();
    return commands;
}
}
