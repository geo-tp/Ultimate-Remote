#ifndef INFRARED_SERVICE_H
#define INFRARED_SERVICE_H

#include <vector>
#include <Models/RemoteCommand.h>
#include <Models/FileRemoteCommand.h>
#include <Contexts/GlobalContext.h>
#include <Repositories/ProtocolRepository.h>
#include <Utils/MakeHexUtils.h>
#include <Utils/StringUtils.h>

namespace services {

    class InfraredService {
    public:
        InfraredService(repositories::ProtocolRepository& protocolRepo);
        void begin();
        void sendRemoteCommand(models::RemoteCommand command, std::string manufacturerName);
        void sendFileRemoteCommand(models::FileRemoteCommand command, std::string remoteName);
    private:
        uint16_t getKaseikyoVendorIdCode(const std::string& input);
        contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
        repositories::ProtocolRepository protocolRepository;
    };

}
#endif