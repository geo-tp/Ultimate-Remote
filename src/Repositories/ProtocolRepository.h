#ifndef PROTOCOL_REPOSITORY_H
#define PROTOCOL_REPOSITORY_H

#include <string>
#include <vector>
#include <Enums/ProtocolEnum.h>
#include <Data/ProtocolDefinitions.h>
#include <Models/Protocol.h>
#include <Data/DefaultFavoriteRemotes.h>

namespace repositories {

    class ProtocolRepository {
    public:
        static const models::Protocol* getAllProtocols();
        static models::Protocol getProtocolByName(const std::string& name);
        static const char* getProtocolString(enums::ProtocolEnum protocol);
        enums::ProtocolEnum getProtocolEnum(const std::string& protocolStr);
    private:
        std::vector<models::Protocol> protocolDefs;
    };
}

#endif // PROTOCOL_REPOSITORY_H
