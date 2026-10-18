#include "ConfirmationSelection.h"
#include <Inputs/InputKeys.h>
#include <Platform/Compatibility/ArduinoCompat.h>

namespace selections {

ConfirmationSelection::ConfirmationSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input)
    : display(display), input(input) {}

bool ConfirmationSelection::select(std::string description, bool protectFromDoubleInput) {
    char key = KEY_NONE;
    display.displayConfirmationPrompt(description);
    if (protectFromDoubleInput) {
        // Keep the prompt visible, but ignore queued/rapid key events (incl. OK)
        // immediately following the Space press that opened it.
        const unsigned long guardStart = millis();
        while (millis() - guardStart < 240) input.handler();
    }

    while (key != KEY_OK && key != KEY_RETURN) {
        key = input.handler();
        if (key == KEY_OK) {
            return true;
        }
    }

    return false;
}

}
