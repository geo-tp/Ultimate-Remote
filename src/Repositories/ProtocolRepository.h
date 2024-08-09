#ifndef PROTOCOL_REPOSITORY_H
#define PROTOCOL_REPOSITORY_H

#include <string>
#include <vector>
#include <Data/ProtocolEnum.h>
#include <Data/ProtocolDefinitions.h>
#include <Models/Protocol.h>

using namespace data;
using namespace models;

namespace repositories {

    class ProtocolRepository {
    public:
        static const Protocol* getAllProtocols();
        static Protocol getProtocolByName(const std::string& name);
        static const char* getProtocolString(ProtocolEnum protocol);
        ProtocolEnum getProtocolEnum(const std::string& protocolStr);
    private:
        std::vector<models::Protocol> protocolDefs;
    };
}

#endif // PROTOCOL_REPOSITORY_H
