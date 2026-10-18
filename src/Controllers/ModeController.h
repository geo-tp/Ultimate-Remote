#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Selections/ModeSelection.h>
#include <Contexts/SelectionContext.h>
#include <Enums/SelectionModeEnum.h>

using namespace selections;
using namespace contexts;
using namespace enums;

namespace controllers {

class ModeController {
public:
    ModeController(interfaces::IDisplayView& display, interfaces::IInputProvider& input, 
                   ModeSelection& modeSelection);

    void handleModeSelection();

private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    ModeSelection& modeSelection;
    SelectionContext& selectionContext = SelectionContext::getInstance();
};

} // namespace controllers

#endif // MODE_CONTROLLER_H
