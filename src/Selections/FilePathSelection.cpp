#include "FilePathSelection.h"

namespace selections {

FilePathSelection::FilePathSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), selectionIndex(0), lastIndex(-1) {}

std::string FilePathSelection::select(const std::vector<std::string>& elementNames, std::string folderPath, uint16_t& selectionIndex) {
    char key = KEY_NONE;
    lastIndex = -1;
    std::vector<std::string> filteredNames = elementNames;
    std::string searchQuery = "";
    std::string upperSearchQuery;
    std::string upperName;
    size_t lastSlashPos;
    std::string folderName = StringUtils::extractFilename(folderPath);
    folderName = folderName == "" ? "root" : folderName; // if foldeName is "" then folderName is "root"
    display.displayTopBar(searchQuery != "" ? searchQuery : folderName, true, true);

    while (key != KEY_OK) {
        if (lastIndex != selectionIndex) {
            display.displaySelection(filteredNames, selectionIndex);
            lastIndex = selectionIndex;
        }

        key = input.handler();

        switch (key) {
            case KEY_NONE:
                break;

            case KEY_OK:
                break;

            case KEY_ARROW_DOWN:
                if (selectionIndex < filteredNames.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = filteredNames.size() - 1;
                }
                break;

            case KEY_RETURN:

                // Trouver la position du dernier slash dans le chemin
                lastSlashPos = folderPath.find_last_of("/\\");

                // Fin de la selection, sortie du mode
                if (lastSlashPos == 0 && folderPath == "/") {
                    return "";
                }

                // si le chemin est à 1 niveau de sous dossier, ex "/folder", retourner "/"
                if (lastSlashPos == 0) {
                    return "/";
                }

                return folderPath.substr(0, lastSlashPos);

            default:
                if (key == KEY_DEL) {
                    if (searchQuery.length() > 0) {
                        searchQuery.pop_back();
                    }
                } else {
                    searchQuery += key;
                    selectionIndex = 0; // Reset the selection after each char entry
                }

                if (searchQuery.empty() == true) {
                    filteredNames = elementNames; // revert to full list
                } else {
                    filteredNames = {};
                    selectionIndex = 0;
                    upperSearchQuery = utils::StringUtils::toUpperCase(searchQuery);

                    // Filter the list based on the search bar, compare uppercase
                    for (const auto& name : elementNames) {
                        upperName = utils::StringUtils::toUpperCase(name);
                        if (upperName.find(upperSearchQuery) != std::string::npos) {
                            filteredNames.push_back(name);
                        }
                    }
                }
                break;

        }

        if (key != KEY_NONE) {
            // Update screen
            display.displayTopBar(searchQuery != "" ? searchQuery : folderName, true, true);
            display.displaySelection(filteredNames, selectionIndex);
        }
    }

    if (folderPath != "/") {
        folderPath += "/";
    }
    
    return folderPath + filteredNames[selectionIndex];

}

}
