#include "RemoteCommandSelection.h"

namespace selections {

RemoteCommandSelection::RemoteCommandSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), confirmationSelection(display, input), stringPromptSelection(display, input), selectionIndex(0), lastIndex(-1) {}

RemoteCommand RemoteCommandSelection::select(const std::vector<RemoteCommand>& commands, 
                                                const std::string& remoteFileName, 
                                                const std::string& productName, 
                                                uint16_t& selectionIndex, 
                                                std::vector<FavoriteRemote> favoriteRemotes,
                                                bool isFavoriteMode,
                                                std::function<std::vector<FavoriteRemote>(const std::string&, const std::string&)> addFavorite, 
                                                std::function<std::vector<FavoriteRemote>(const std::string&)> deleteFavorite,
                                                RemoteCommand emptyRemoteCommand) {

    char key = KEY_NONE;
    lastIndex = -1;
    std::vector<char> commandMappings = input.getCommandMappings(commands);
    std::vector<std::string> commandNames = utils::StringUtils::extractFieldNames(commands, "functionName");

    // Copie locale des remotes pour maintenir a jour quand save/delete
    std::vector<FavoriteRemote> currentFavoriteRemotes = favoriteRemotes;

    display.displayTopBar("Remote " + remoteFileName, true);

    while (key != KEY_OK) {

        if (lastIndex != selectionIndex) {
            display.displaySelection(
                                commandNames, selectionIndex, 
                                utils::StringUtils::convertCharVectorToStringVector(commandMappings));
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

            case KEY_FAVORITE: {
                bool isFavoriteRemote = false;
                const char* favName;
                for (const auto& fav : currentFavoriteRemotes) {
                    if (strcmp(fav.favoriteName, remoteFileName.c_str()) == 0 &&
                        strcmp(fav.productName, productName.c_str()) == 0) {
                        isFavoriteRemote = true;
                        favName = fav.favoriteName;
                        break;
                    }
                }

                // Delete from favorite
                if (isFavoriteRemote) {
                    bool confirmation = confirmationSelection.select("Delete favorite ?");

                    if (confirmation) {
                        currentFavoriteRemotes = deleteFavorite(favName);
                        return emptyRemoteCommand;
                    }
                    lastIndex = -1; // hack to update render

                // Save to Favorite
                } else if (!isFavoriteMode) {
                    std::string favoriteName = stringPromptSelection.select("Type a favorite name");

                    if (!favoriteName.empty()) {
                        currentFavoriteRemotes = addFavorite(remoteFileName, favoriteName);
                    }
                    lastIndex = -1; // hack to update render
                }
                break;
            }

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
