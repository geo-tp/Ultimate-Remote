#include "SdService.h"
namespace services {
SdService::SdService() = default;
bool SdService::getSdState() { return sdCardMounted; }
}
