#include "ProtocolService.h"

namespace services {

ProtocolService::ProtocolService() : protocolRepository() {}

ProtocolService::~ProtocolService() {}


const char* ProtocolService::getProtocolString(ProtocolEnum protocol) {
    return protocolRepository.getProtocolString(protocol);
}

ProtocolEnum ProtocolService::getProtocolEnum(std::string protocolString) {
    return protocolRepository.getProtocolEnum(protocolString);
}


}