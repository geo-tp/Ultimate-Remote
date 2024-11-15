#ifndef INFRARED_SERVICE_H
#define INFRARED_SERVICE_H

#include <vector>
#include <Models/RemoteCommand.h>
#include <Models/FileRemoteCommand.h>
#include <Contexts/GlobalContext.h>
#include <Repositories/ProtocolRepository.h>
#include <Utils/MakeHexUtils.h>
#include <Utils/StringUtils.h>

using namespace models;
using namespace contexts;
using namespace utils;
using namespace repositories;

namespace services {

    class InfraredService {
    public:
        InfraredService(GlobalContext& context, ProtocolRepository& protocolRepo);
        void begin();
        void sendRemoteCommand(RemoteCommand command, std::string manufacturerName);
        void sendFileRemoteCommand(FileRemoteCommand command, std::string remoteName);
    private:
        uint16_t getKaseikyoVendorIdCode(const std::string& input);
        GlobalContext& globalContext = GlobalContext::getInstance();
        ProtocolRepository protocolRepository;
    };

}
#endif