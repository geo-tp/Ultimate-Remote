#ifndef REMOTE_SELECTION_H
#define REMOTE_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

class RemoteSelection {
public:
    RemoteSelection(CardputerView& display, CardputerInput& input);
    Remote select(const std::vector<Remote>& remotes, const std::string& productName, uint16_t& selectionIndex, Remote emptyRemote);
private:
    GlobalContext& context = GlobalContext::getInstance();
    CardputerView& display;
    CardputerInput& input;
    uint8_t selectionIndex;
    int8_t lastIndex;
};

}

#endif