#include "RemoteCommandSelection.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedCommandInput.h>
#include <Inputs/ScopedZBack.h>
#include <Inputs/CommandShortcuts.h>

namespace selections {

RemoteCommandSelection::RemoteCommandSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input)
    : display(display), input(input), confirmationSelection(display, input), stringPromptSelection(display, input), lastIndex(-1) {}

RemoteCommand RemoteCommandSelection::select(const std::vector<RemoteCommand>& commands, 
                                                const std::string& remoteFileName, 
                                                const std::string& productName, 
                                                uint16_t& selectionIndex, 
                                                std::vector<FavoriteRemote> favoriteRemotes,
                                                bool isFavoriteMode,
                                                std::function<std::vector<FavoriteRemote>(const std::string&, const std::string&)> addFavorite, 
                                                std::function<std::vector<FavoriteRemote>(const std::string&)> deleteFavorite,
                                                RemoteCommand emptyRemoteCommand) {
    inputs::ScopedZBack zBack(input);
    inputs::ScopedCommandInput commandInput(input);

    char key = KEY_NONE;
    lastIndex = -1;
    std::vector<char> commandMappings = inputs::getCommandMappings(commands, input);
    std::vector<std::string> commandNames = utils::StringUtils::extractFieldNames(commands, "functionName");

    // Copie locale des remotes pour maintenir a jour quand save/delete
    std::vector<FavoriteRemote> currentFavoriteRemotes = favoriteRemotes;

    display.displayTopBar("Remote " + remoteFileName +
                          (productName.empty() ? "" : " - " + productName), true);

    while (key != KEY_OK) {

        if (lastIndex != selectionIndex) {
            display.displaySelection(
                                commandNames, selectionIndex, 
                                inputs::getCommandLabels(commandMappings));
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

            // Favorites are managed from the remote list, not command lists.
            case KEY_FAVORITE:
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
