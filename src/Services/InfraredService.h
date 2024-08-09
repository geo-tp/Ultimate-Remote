#ifndef INFRARED_SERVICE_H
#define INFRARED_SERVICE_H

#include <vector>
#include <Models/RemoteCommand.h>
#include <Contexts/GlobalContext.h>
#include <Utils/MakeHexUtils.h>

using namespace models;
using namespace contexts;
using namespace utils;

namespace services {

    class InfraredService {
    public:
        InfraredService();
        void begin();
        void sendRemoteCommand(RemoteCommand command, const char* protocolString);
    private:
        GlobalContext& context = GlobalContext::getInstance();
    };

}
#endif