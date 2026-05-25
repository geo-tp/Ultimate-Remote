#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <sstream>
#include <vector>
#include <string>
#include <Models/FileRemote.h>
#include <Models/FileRemoteCommand.h>
#include <Repositories/ProtocolRepository.h>
#include <Utils/StringUtils.h>

namespace services {

    class FileService {
    public:
        explicit FileService(repositories::ProtocolRepository& protocolRepo);

        models::FileRemote getRemoteFromFile(const std::string& fileName, const std::string& fileContent);
        bool validateInfraredFile(const std::string& fileContent);

    private:
        std::vector<models::FileRemoteCommand> parseInfraredFile(const std::string& fileContent);

        repositories::ProtocolRepository& protocolRepository;
    };
}

#endif // FILE_SERVICE_H
