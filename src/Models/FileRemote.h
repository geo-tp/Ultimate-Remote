#ifndef FILE_REMOTE_MODEL_H
#define FILE_REMOTE_MODEL_H

#include <stddef.h>
#include <cstring>
#include <vector>
#include <Models/FileRemoteCommand.h>

namespace models {

    typedef struct {
        std::string fileName;
        std::vector<FileRemoteCommand> commands;
    } FileRemote;
}

#endif // REMOTE_H
