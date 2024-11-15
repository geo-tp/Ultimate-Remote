#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <sstream>
#include <vector>
#include <string>
#include <Models/FileRemote.h>
#include <Models/FileRemoteCommand.h>
#include <Repositories/ProtocolRepository.h>
#include <Utils/StringUtils.h>

using namespace models;
using namespace repositories;
using namespace utils;

namespace services {

    class FileService {
    public:
        explicit FileService(ProtocolRepository& protocolRepo);

        FileRemote getRemoteFromFile(const std::string& fileName, const std::string& fileContent);
        bool validateInfraredFile(const std::string& fileContent);

    private:
        std::vector<FileRemoteCommand> parseInfraredFile(const std::string& fileContent);

        // Protocole Repository injecté
        ProtocolRepository& protocolRepository;
    };
}

#endif // FILE_SERVICE_H
