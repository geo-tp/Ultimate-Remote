#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Selections/ModeSelection.h>
#include <Contexts/SelectionContext.h>
#include <Enums/SelectionModeEnum.h>

namespace controllers {

class ModeController {
public:
    ModeController(views::CardputerView& display, inputs::CardputerInput& input,
                   selections::ModeSelection& modeSelection);

    void handleModeSelection();

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    selections::ModeSelection& modeSelection;
    contexts::SelectionContext& selectionContext = contexts::SelectionContext::getInstance();
};

} // namespace controllers

#endif // MODE_CONTROLLER_H
