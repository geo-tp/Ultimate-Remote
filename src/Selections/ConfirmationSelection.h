#ifndef CONFIRMATION_SELECTION_H
#define CONFIRMATION_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Contexts/GlobalContext.h>

using namespace contexts;

namespace selections {

class ConfirmationSelection {
public:
    ConfirmationSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    bool select(std::string description, bool protectFromDoubleInput = false);
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
};

}

#endif