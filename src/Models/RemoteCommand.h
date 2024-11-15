
#ifndef REMOTE_COMMAND_MODEL_H
#define REMOTE_COMMAND_MODEL_H

#include <stdint.h>
#include <Enums/ProtocolEnum.h>
#include <string.h>

using namespace enums;

namespace models {

    typedef struct {
        const char* functionName;
        ProtocolEnum protocol;
        int16_t device;  // peut etre negatif
        int16_t subdevice;   // peut etre negatif 
        int16_t function;  
    } RemoteCommand;

    inline bool operator==(const RemoteCommand& lhs, const RemoteCommand& rhs) {
        return (strcmp(lhs.functionName, rhs.functionName) == 0) && (lhs.protocol == rhs.protocol) && (lhs.device == rhs.device) && (lhs.subdevice == rhs.subdevice) && (lhs.function == rhs.function);
    }

    inline bool operator!=(const RemoteCommand& lhs, const RemoteCommand& rhs) {
        return !(lhs == rhs);
    }

}

#endif