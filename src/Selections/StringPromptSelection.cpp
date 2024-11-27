#include "StringPromptSelection.h"

namespace selections {

StringPromptSelection::StringPromptSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

std::string StringPromptSelection::select(std::string description) {
    std::string output;
    char key = KEY_NONE;
    display.displayStringPrompt(description, output);

    while (key != KEY_OK || output.empty()) {
        key = input.handler();
        if (key == KEY_DEL) {
            if (!output.empty()) {
                output.pop_back();
            }
        }
        else if (key == KEY_RETURN) {
            return ""; // empty string will not save the remote
        }
        else if (isalnum(key) && output.size() < globalContext.getMaxInputCharCount()) {
            output += key;
        }

        if (key != KEY_NONE) {
            display.displayStringPrompt(description, output);
        }
    }

    return output;
}

}