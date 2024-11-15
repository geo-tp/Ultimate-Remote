#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Selections/ModeSelection.h>
#include <Contexts/SelectionContext.h>
#include <Enums/SelectionModeEnum.h>

using namespace views;
using namespace inputs;
using namespace selections;
using namespace contexts;
using namespace enums;

namespace controllers {

class ModeController {
public:
    ModeController(CardputerView& display, CardputerInput& input, 
                   GlobalContext& globalContext, SelectionContext& selectionContext, 
                   ModeSelection& modeSelection);

    void handleModeSelection();

private:
    CardputerView& display;
    CardputerInput& input;
    ModeSelection& modeSelection;
    GlobalContext& globalContext;
    SelectionContext& selectionContext;
};

} // namespace controllers

#endif // MODE_CONTROLLER_H
