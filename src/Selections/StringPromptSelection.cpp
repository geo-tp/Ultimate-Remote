#include "StringPromptSelection.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedLiteralTextInput.h>

namespace selections {

StringPromptSelection::StringPromptSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input)
    : display(display), input(input) {}

std::string StringPromptSelection::select(std::string description) {
    inputs::ScopedLiteralTextInput textInput(input);
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
        else if (((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                  (key >= '0' && key <= '9')) && output.size() < globalContext.getMaxInputCharCount()) {
            output += key;
        }

        if (key != KEY_NONE) {
            display.displayStringPrompt(description, output);
        }
    }

    return output;
}

}
