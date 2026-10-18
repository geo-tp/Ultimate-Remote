#ifndef INFRARED_SERVICE_H
#define INFRARED_SERVICE_H

#include <vector>
#include <Models/RemoteCommand.h>
#include <Models/FileRemoteCommand.h>
#include <Contexts/GlobalContext.h>
#include <Repositories/ProtocolRepository.h>
#include <Interfaces/IInfraredSender.h>
#include <Enums/InfraredSendResultEnum.h>

using namespace models;
using namespace contexts;
using namespace repositories;

namespace services {

    class InfraredService {
    public:
        InfraredService(ProtocolRepository& protocolRepo, interfaces::IInfraredSender& sender);
        void begin();
        bool sendRemoteCommand(RemoteCommand command, std::string manufacturerName);
        enums::InfraredSendResultEnum sendRemoteCommandChecked(const RemoteCommand& command);
        bool sendFileRemoteCommand(FileRemoteCommand command, std::string remoteName);
    private:
        bool fileToggle = false;
        GlobalContext& globalContext = GlobalContext::getInstance();
        ProtocolRepository protocolRepository;
        interfaces::IInfraredSender& sender;
    };

}
#endif
