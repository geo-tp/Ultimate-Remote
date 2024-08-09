#ifndef PROTOCOL_SERVICE_H
#define PROTOCOL_SERVICE_H

#include <vector>
#include <string>
#include <Repositories/ProtocolRepository.h>

using namespace models;
using namespace repositories;

namespace services {

    class ProtocolService {
    public:
        ProtocolService();
        ~ProtocolService();

        const char* getProtocolString(ProtocolEnum protocol);
        ProtocolEnum getProtocolEnum(const std::string protocolStr);
    private:
        ProtocolRepository protocolRepository;
    };

} // namespace services

#endif // REMOTE_SERVICE_H
