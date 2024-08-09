
#ifndef FILE_REMOTE_COMMAND_MODEL_H
#define FILE_REMOTE_COMMAND_MODEL_H

#include <stddef.h>
#include <string>
#include <Data/ProtocolEnum.h>

using namespace data;

namespace models {

    typedef struct {
        std::string commandName;
        ProtocolEnum protocol;
        uint16_t address;
        uint8_t command;
        uint16_t* rawData; // unused if protocol != RAW
        size_t rawDataSize; // unused if protocol != RAW
        int frequency; // unused if protocol != RAW
        float dutyCycle; // unused if protocol != RAW
    } FileRemoteCommand;

}

#endif