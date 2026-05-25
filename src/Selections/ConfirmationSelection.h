#ifndef CONFIRMATION_SELECTION_H
#define CONFIRMATION_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

namespace selections {

class ConfirmationSelection {
public:
    ConfirmationSelection(views::CardputerView& display, inputs::CardputerInput& input);
    bool select(std::string description);
private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
};

}

#endif