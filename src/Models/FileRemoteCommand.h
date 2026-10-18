#ifndef FILE_REMOTE_COMMAND_MODEL_H
#define FILE_REMOTE_COMMAND_MODEL_H
#include <cstdint>
#include <string>
#include <vector>
#include <Enums/ProtocolEnum.h>

namespace models {
struct FileRemoteCommand {
    std::string functionName;
    enums::ProtocolEnum protocol = enums::ProtocolEnum(0);
    uint32_t address = 0;
    uint32_t function = 0;
    std::vector<uint32_t> rawData;
    int frequency = 38000;
    float dutyCycle = 0.33f;
    std::string fileProtocol;
    std::string error;
};
inline bool operator==(const FileRemoteCommand& a, const FileRemoteCommand& b) {
    return a.functionName == b.functionName && a.protocol == b.protocol &&
           a.address == b.address && a.function == b.function && a.rawData == b.rawData &&
           a.frequency == b.frequency && a.dutyCycle == b.dutyCycle &&
           a.fileProtocol == b.fileProtocol && a.error == b.error;
}
inline bool operator!=(const FileRemoteCommand& a, const FileRemoteCommand& b) { return !(a == b); }
}
#endif
