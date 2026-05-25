#ifndef REMOTE_SELECTION_H
#define REMOTE_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Models/Remote.h>

namespace selections {

class RemoteSelection {
public:
    RemoteSelection(views::CardputerView& display, inputs::CardputerInput& input);
    models::Remote select(const std::vector<models::Remote>& remotes, const std::string& productName, uint16_t& selectionIndex, models::Remote emptyRemote);
private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    int16_t lastIndex;
};

}

#endif