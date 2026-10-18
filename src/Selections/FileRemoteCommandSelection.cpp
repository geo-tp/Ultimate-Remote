#include "FileRemoteCommandSelection.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedCommandInput.h>
#include <Inputs/ScopedZBack.h>
#include <Inputs/CommandShortcuts.h>

namespace selections {

FileRemoteCommandSelection::FileRemoteCommandSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input)
    : display(display), input(input) {}

FileRemoteCommand FileRemoteCommandSelection::select(const std::vector<FileRemoteCommand>& commands, 
                                                const std::string& remoteName, 
                                                uint16_t& selectionIndex, 
                                                FileRemoteCommand emptyRemoteCommand) {
    inputs::ScopedZBack zBack(input);
    inputs::ScopedCommandInput commandInput(input);
    char key = KEY_NONE;
    lastIndex = -1;
    std::vector<char> commandMappings = inputs::getCommandMappings(commands, input);
    std::vector<std::string> commandNames = utils::StringUtils::extractFieldNames(commands, "functionName");

    display.displayTopBar(remoteName, true);
    if (commands.empty()) return emptyRemoteCommand;
    if (selectionIndex >= commands.size()) selectionIndex = 0;

    while (key != KEY_OK) {

        if (lastIndex != selectionIndex) {
            display.displaySelection(
                commandNames, selectionIndex, 
                inputs::getCommandLabels(commandMappings)
            );
            lastIndex = selectionIndex;
        }

        key = input.handler();

        switch (key) {
            case KEY_NONE:
                break;

            case KEY_ARROW_DOWN:
                if (selectionIndex < commands.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = commands.size() - 1;
                }
                break;

            case KEY_RETURN:
                return emptyRemoteCommand;

            default:
                // We check for keybinds
                for (int i = 0; i < commandMappings.size() ; i++) {
                    if (key == commandMappings[i]) {
                        return commands[i];
                    }
                }
                break;
        }
    }

    return commands[selectionIndex];
}


}
