#include "ModeController.h"
#include <Inputs/InputKeys.h>
#include <Platform/Compatibility/ArduinoCompat.h>

namespace controllers {

ModeController::ModeController(interfaces::IDisplayView& display, interfaces::IInputProvider& input, ModeSelection& modeSelection)
    : display(display), input(input), modeSelection(modeSelection) {}

void ModeController::handleModeSelection() {
    SelectionModeEnum selectedMode = modeSelection.select();
    // A previous mode (especially Favorites) can leave these flags set.
    // Never reuse that navigation context for a fresh Remotes/Scan session.
    selectionContext.setIsManufacturerSelected(false);
    selectionContext.setIsProductSelected(false);
    selectionContext.setIsRemoteSelected(false);
    selectionContext.setIsFileRemoteSelected(false);
    selectionContext.setCurrentManufacturerIndex(0);
    selectionContext.setCurrentProductIndex(0);
    selectionContext.setCurrentRemoteIndex(0);
    selectionContext.setCurrentRemoteCommandIndex(0);
    selectionContext.setCurrentFileRemoteCommandIndex(0);
    selectionContext.setCurrentSelectedMode(selectedMode);
    selectionContext.setIsModeSelected(true);

    if (selectedMode == SelectionModeEnum::SCAN) {
        display.displayScanInfo();
        char key = KEY_NONE;
        while (key != KEY_OK && key != KEY_RETURN) key = input.handler();
        if (key == KEY_RETURN) {
            selectionContext.setIsModeSelected(false);
            return;
        }
        // Swallow any queued OK/Space/arrow input from dismissing the guide.
        // Without this, a rapid second key can choose the first manufacturer.
        const unsigned long guardStart = millis();
        while (millis() - guardStart < 240) input.handler();
    }

    if (selectedMode == SelectionModeEnum::FAVORITES) {
        selectionContext.setIsManufacturerSelected(true);
        selectionContext.setIsProductSelected(true);
    }
}

} // namespace controllers
