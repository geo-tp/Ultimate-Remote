#include "ModeController.h"

namespace controllers {

ModeController::ModeController(CardputerView& display, CardputerInput& input, GlobalContext& globalContext, 
                               SelectionContext& selectionContext, ModeSelection& modeSelection)
    : display(display), input(input), globalContext(globalContext), selectionContext(selectionContext), modeSelection(modeSelection) {}

void ModeController::handleModeSelection() {
    SelectionModeEnum selectedMode = modeSelection.select();
    selectionContext.setCurrentSelectedMode(selectedMode);
    selectionContext.setIsModeSelected(true);

    if (selectedMode == SelectionModeEnum::FAVORITES) {
        selectionContext.setIsManufacturerSelected(true);
        selectionContext.setIsProductSelected(true);
    }
}

} // namespace controllers
