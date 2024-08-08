#include "ConfirmationSelection.h"

namespace selections {

ConfirmationSelection::ConfirmationSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

bool ConfirmationSelection::select(std::string description) {
    char key = KEY_NONE;
    display.displayConfirmationPrompt(description);

    while (key != KEY_OK && key != KEY_RETURN ) {
        key = input.handler();
        if (key == KEY_OK) {
            return true;
        }
    }

    return false;
}

}