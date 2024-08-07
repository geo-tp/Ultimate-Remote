#ifndef CONFIRMATION_SELECTION_H
#define CONFIRMATION_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

class ConfirmationSelection {
public:
    ConfirmationSelection(CardputerView& display, CardputerInput& input);
    bool select(std::string description);
private:
    CardputerView& display;
    CardputerInput& input;
};

}

#endif