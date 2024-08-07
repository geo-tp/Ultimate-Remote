#ifndef REMOTE_MODEL_H
#define REMOTE_MODEL_H

#include <stddef.h>
#include <cstring>
#include <Models/Remote.h>
#include <Models/RemoteCommand.h>

namespace models {

    typedef struct {
        char fileName[14];
        const RemoteCommand *commands;
        size_t commandCount;
    } Remote;

    inline bool operator==(const Remote& lhs, const Remote& rhs) {
        return (strcmp(lhs.fileName, rhs.fileName) == 0) &&
            (lhs.commands == rhs.commands) &&
            (lhs.commandCount == rhs.commandCount);
    }

    inline bool operator!=(const Remote& lhs, const Remote& rhs) {
        return !(lhs == rhs);
    }
}

#endif // REMOTE_H
