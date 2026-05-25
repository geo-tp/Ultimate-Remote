#ifndef SCAN_SELECTION_H
#define SCAN_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/StringPromptSelection.h>
#include <Models/Remote.h>
#include <Models/RemoteCommand.h>

namespace selections {

class ScanSelection {
public:
    ScanSelection(views::CardputerView& display, inputs::CardputerInput& input);
    models::Remote select(const std::vector<models::Remote>& remotes,
                  const std::string manufacturerName,
                  std::function<void(const models::RemoteCommand&, std::string)> sendCommand,
                  std::string& favoriteName, bool last, models::Remote emptyRemote);
private:
    ConfirmationSelection confirmationSelection;
    StringPromptSelection stringPromptSelection;
    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    views::CardputerView& display;
    inputs::CardputerInput& input;
    int16_t lastIndex;
};

}

#endif