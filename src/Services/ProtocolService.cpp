#include "ProtocolService.h"

namespace services {

ProtocolService::ProtocolService() : protocolRepository() {}

ProtocolService::~ProtocolService() {}


const char* ProtocolService::getProtocolString(ProtocolEnum protocol) {
    return protocolRepository.getProtocolString(protocol);
}

}