/*

############################################################################################################################################
#                                                                                                                                          #
#                                                                                                                                          #
#                              Controls the selection of different screens and returns the selected struct                                 #
#                                In the case of user hits return button, the returned structure is empty                                   #           
#                                                                                                                                          #
#                                                                                                                                          #
############################################################################################################################################

*/


#include "selection.h"

Selection::Selection(DisplayManager& displayManager, CustomKeyboard& keyboard,
                     const Manufacturer& emptyManufacturer,
                     const Product& emptyProduct,
                     const Remote& emptyRemote,
                     const RemoteCommand& emptyRemoteCommand)
    : displayManager(displayManager), keyboard(keyboard),
      emptyManufacturer(emptyManufacturer), emptyProduct(emptyProduct),
      emptyRemote(emptyRemote), emptyRemoteCommand(emptyRemoteCommand) {}

SelectionMode Selection::modeSelection() {
    input = KEY_NONE;
    selectionIndex = 0;
    lastIndex = -1;
    SelectionMode selectionMode = static_cast<SelectionMode>(0); // first element

    // Main title
    displayManager.displayTopBar("Ultimate Remote");

    while (input != KEY_OK) {

        if (lastIndex != selectionIndex) {
            displayManager.displaySelection(getSelectionModeStrings(), selectionIndex, getSelectionModeDescriptionStrings());
            lastIndex = selectionIndex;
        }

        input = keyboard.handler();

        switch (input) {
            case KEY_ARROW_DOWN:
                if (selectionIndex < (uint8_t)SelectionMode::COUNT - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = (uint8_t)SelectionMode::COUNT - 1;
                }
                break;
            default:
                break;
        }
    }
    return static_cast<SelectionMode>(selectionIndex);
}

std::string Selection::manufacturerSelection(const std::vector<std::string>& manufacturerNames, uint16_t& selectionIndex, bool scan) {
    input = KEY_NONE;
    lastIndex = -1;
    std::vector<std::string> filteredManufacturerNames = manufacturerNames;
    std::string searchQuery = "";
    std::string upperSearchQuery;
    std::string upperManufacturerName;

    if (scan) {
        displayManager.displayScanInfo();
        while (input != KEY_OK) {
            input = keyboard.handler();
        }

        input = KEY_NONE;
    }

    displayManager.displayTopBar(searchQuery, true, true); // searchbar
    displayManager.displaySelection(filteredManufacturerNames, selectionIndex);

    while (input != KEY_OK) {
        input = keyboard.handler();

        switch (input) {
            case KEY_NONE:
                break;

            case KEY_OK:
                break;

            case KEY_ARROW_DOWN:
                if (selectionIndex < filteredManufacturerNames.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = filteredManufacturerNames.size() - 1;
                }
                break;
            case KEY_RETURN:
                return ""; // empty return

            default:
                if (input == KEY_DEL) {
                    if (searchQuery.length() > 0) {
                        searchQuery.pop_back();
                    }
                } else {
                    searchQuery += input;
                    selectionIndex = 0; // Reset the selection after each char entry
                }

                if (searchQuery.empty() == true) {
                    filteredManufacturerNames = manufacturerNames; // revert to full list
                } else {
                    filteredManufacturerNames = {};
                    selectionIndex = 0;
                    upperSearchQuery = Utils::toUpperCase(searchQuery);

                    // Filter the list of manufacturers based on the search bar, compare uppercase
                    for (const auto& name : manufacturerNames) {
                        upperManufacturerName = Utils::toUpperCase(name);
                        if (upperManufacturerName.find(upperSearchQuery) != std::string::npos) {
                            filteredManufacturerNames.push_back(name);
                        }
                    }
                }
                break;
        }

        if (input != KEY_NONE) {
            // Update screen
            displayManager.displayTopBar(searchQuery, true, true);
            displayManager.displaySelection(filteredManufacturerNames, selectionIndex);
        }
    }

    return filteredManufacturerNames[selectionIndex];
}

Product Selection::productSelection(const std::vector<Product>& products, const std::string& manufacturerName, uint16_t& selectionIndex) {
    input = KEY_NONE;
    lastIndex = -1;

    displayManager.displayTopBar(manufacturerName, true);
    std::vector<std::string> productNames = Utils::extractFieldNames(products, "name");

    while (input != KEY_OK) {
        if (lastIndex != selectionIndex) {
            displayManager.displaySelection(productNames, selectionIndex);
            lastIndex = selectionIndex;
        }

        input = keyboard.handler();

        switch (input) {
            case KEY_NONE:
                break;

            case KEY_ARROW_DOWN:
                if (selectionIndex < products.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = products.size() - 1;
                }
                break;

            case KEY_RETURN:
                return emptyProduct;

            default:
                break;
        }
    }

    return products[selectionIndex];
}

Remote Selection::remoteSelection(const std::vector<Remote>& remotes, const std::string& productName, uint16_t& selectionIndex) {
    input = KEY_NONE;
    lastIndex = -1;
    std::vector<std::string> remoteNames = Utils::extractFieldNames(remotes, "fileName");
    remoteNames = Utils::addRemotesPrefixes(remoteNames);

    displayManager.displayTopBar(productName, true);

    while (input != KEY_OK) {
        if (lastIndex != selectionIndex) {
            displayManager.displaySelection(remoteNames, selectionIndex);
            lastIndex = selectionIndex;
        }

        input = keyboard.handler();

        switch (input) {
            case KEY_NONE:
                break;

            case KEY_ARROW_DOWN:
                if (selectionIndex < remotes.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = remotes.size() - 1;
                }
                break;

            case KEY_RETURN:
                return emptyRemote;

            default:
                break;
        }
    }

    return remotes[selectionIndex];
}

RemoteCommand Selection::remoteCommandSelection(const std::vector<RemoteCommand>& commands, 
                                                const std::string& remoteFileName, 
                                                const std::string& productName, 
                                                uint16_t& selectionIndex, 
                                                std::vector<FavoriteRemote> favoriteRemotes,
                                                bool isFavoriteMode,
                                                std::function<std::vector<FavoriteRemote>(const std::string&, const std::string&)> addFavorite, 
                                                std::function<std::vector<FavoriteRemote>(const std::string&)> deleteFavorite) {
    
    input = KEY_NONE;
    lastIndex = -1;
    std::vector<char> commandMappings = keyboard.getCommandMappings(commands);
    std::vector<std::string> commandNames = Utils::extractFieldNames(commands, "functionName");

    // Copie locale des remotes pour maintenir a jour quand save/delete
    std::vector<FavoriteRemote> currentFavoriteRemotes = favoriteRemotes;

    displayManager.displayTopBar("Remote " + remoteFileName, true);

    while (input != KEY_OK) {

        if (lastIndex != selectionIndex) {
            displayManager.displaySelection(
                                commandNames, selectionIndex, 
                                Utils::convertCharVectorToStringVector(commandMappings));
            lastIndex = selectionIndex;
        }

        input = keyboard.handler();

        switch (input) {
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
                    bool confirmation = confirmationSelection("Delete favorite ?");

                    if (confirmation) {
                        currentFavoriteRemotes = deleteFavorite(favName);
                        return emptyRemoteCommand;
                    }
                    lastIndex = -1; // hack to update render

                // Save to Favorite
                } else if (!isFavoriteMode) {
                    std::string favoriteName = stringPromptSelection("Type a favorite name");

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
                    if (input == commandMappings[i]) {
                        return commands[i];
                    }
                }
                break;
        }
    }

    return commands[selectionIndex];
}

Remote Selection::scanSelection(
    const std::vector<Remote>& remotes, 
    std::string manufacturerName, 
    std::function<void(const RemoteCommand&)> sendCommand, 
    std::string& favoriteName, bool last) {

    input = KEY_NONE;
    const int intervalMs =  180; // pour input utilisateur

    displayManager.displayTopBar("Remotes Scan ");
    displayManager.displayScanSelection(manufacturerName);

    for (const auto& remote : remotes) {
        displayManager.displayScanRemote(remote.fileName);
        for (size_t i = 0; i < remote.commandCount; ++i) {
            sendCommand(remote.commands[i]);
            displayManager.displayScanCommand(remote.commands[i].functionName);
            // Vérifier l'input utilisateur
            unsigned long startTime = millis();
            while (millis() - startTime < intervalMs) {
                input = keyboard.handler();
                if (input == KEY_FAVORITE) {
                    favoriteName = stringPromptSelection("Type a favorite name");
                    return remote; // Retourner la remote selectionné
                }
            }
        }
    }

    // Last element, we display the end of the scan
    if (last) {
        displayManager.displayScanOver();
        while (input != KEY_OK) {
            input = keyboard.handler();
        }
    }

    return emptyRemote;
}

std::string Selection::stringPromptSelection(std::string description) {
    std::string output;
    input = KEY_NONE;
    displayManager.displayStringPrompt(description, output);

    while (input != KEY_OK || output.empty()) {
        input = keyboard.handler();
        if (input == KEY_DEL) {
            if (!output.empty()) {
                output.pop_back();
            }
        }
        else if (input == KEY_RETURN) {
            return ""; // empty string will not save the remote
        }
        else if (isalnum(input) && output.size() < MAX_STRING_INPUT) {
            output += input;
        }

        if (input != KEY_NONE) {
            displayManager.displayStringPrompt(description, output);
        }
    }

    return output;
}

bool Selection::confirmationSelection(std::string description) {
    std::string output;
    input = KEY_NONE;
    displayManager.displayConfirmationPrompt(description);

    while (input != KEY_OK && input != KEY_RETURN ) {
        input = keyboard.handler();
        if (input == KEY_OK) {
            return true;
        }
    }

    return false;
}

const std::string getSelectionModeToString(SelectionMode mode) {
    switch (mode) {
        case SelectionMode::SCAN:
            return "SCANNER";
        case SelectionMode::FAVORITES:
            return "FAVORITES";
        case SelectionMode::POPULARS:
            return "POPULARS";
        case SelectionMode::ALL_REMOTES:
            return "ALL REMOTES";
        default:
            return "UNKNOWN";
    }
}

const std::string getSelectionModeDescription(SelectionMode mode) {
    switch (mode) {
        case SelectionMode::SCAN:
            return "   search for remotes";
        case SelectionMode::FAVORITES:
            return "saved remotes";
        case SelectionMode::POPULARS:
            return "     remote brands";
        case SelectionMode::ALL_REMOTES:
            return "available";
        default:
            return "UNKNOWN";
    }
}

const std::vector<std::string> getSelectionModeStrings() {
    std::vector<std::string> modeStrings;
    for (int i = 0; i < static_cast<int>(SelectionMode::COUNT); ++i) {
        modeStrings.push_back(getSelectionModeToString(static_cast<SelectionMode>(i)));
    }
    return modeStrings;
}

const std::vector<std::string> getSelectionModeDescriptionStrings() {
    std::vector<std::string> descriptionStrings;
    for (int i = 0; i < static_cast<int>(SelectionMode::COUNT); ++i) {
        descriptionStrings.push_back(getSelectionModeDescription(static_cast<SelectionMode>(i)));
    }
    return descriptionStrings;
}


