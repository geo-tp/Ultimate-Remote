#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <sstream>
#include <vector>
#include <string>
#include <Models/FileRemoteCommand.h>
#include <Repositories/ProtocolRepository.h>
#include <Utils/StringUtils.h>

using namespace models;
using namespace repositories;
using namespace utils;

namespace services {

    class FileService {
    public:
        static std::vector<FileRemoteCommand> parseIrFile(const std::string& fileContent);
    private:
        static ProtocolRepository protocolRepository;

    };
}


#endif // FILE_SERVICE_H
