#ifndef SCAN_SELECTION_H
#define SCAN_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/StringPromptSelection.h>

using namespace views;
using namespace inputs;
using namespace contexts;
using namespace selections;

namespace selections {

class ScanSelection {
public:
    ScanSelection(CardputerView& display, CardputerInput& input, GlobalContext& globalContext);
    Remote select(const std::vector<Remote>& remotes, 
                  const std::string manufacturerName, 
                  std::function<void(const RemoteCommand&, std::string)> sendCommand, 
                  std::string& favoriteName, bool last, Remote emptyRemote);
private:
    ConfirmationSelection confirmationSelection;
    StringPromptSelection stringPromptSelection;
    GlobalContext& globalContext;
    CardputerView& display;
    CardputerInput& input;
    int16_t lastIndex;
};

}

#endif