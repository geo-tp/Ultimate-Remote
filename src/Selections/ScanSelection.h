#ifndef SCAN_SELECTION_H
#define SCAN_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Models/ScanRemoteTarget.h>
#include <Contexts/GlobalContext.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/StringPromptSelection.h>
#include <functional>
#include <Enums/InfraredSendResultEnum.h>

using namespace contexts;
using namespace models;

namespace selections {

class ScanSelection {
public:
    ScanSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    Remote select(const std::vector<ScanRemoteTarget>& targets,
                  const std::string manufacturerName, 
                  std::function<enums::InfraredSendResultEnum(const RemoteCommand&, std::string)> sendCommand,
                  std::string& favoriteName,
                  std::string& selectedProductName,
                  Remote emptyRemote);
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    ConfirmationSelection confirmationSelection;
    StringPromptSelection stringPromptSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();
    int16_t lastIndex;
};

}

#endif
