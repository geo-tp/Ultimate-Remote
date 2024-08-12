#include "FilePathSelection.h"

namespace selections {

FilePathSelection::FilePathSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), selectionIndex(0), lastIndex(-1) {}

std::string FilePathSelection::select(const std::vector<std::string>& elementNames, std::string folderPath, uint16_t& selectionIndex) {
    char key = KEY_NONE;
    lastIndex = -1;
    size_t lastSlashPos;
    std::string folderName = StringUtils::extractFilename(folderPath);
    folderName = folderName == "" ? "root" : folderName; // if foldeName is "" then folderName is "root"
    display.displayTopBar(folderName, true);

    while (key != KEY_OK) {
        if (lastIndex != selectionIndex) {
            display.displaySelection(elementNames, selectionIndex);
            lastIndex = selectionIndex;
        }

        key = input.handler();

        switch (key) {
            case KEY_NONE:
                break;

            case KEY_ARROW_DOWN:
                if (selectionIndex < elementNames.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = elementNames.size() - 1;
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
                break;
        }
    }

    if (folderPath != "/") {
        folderPath += "/";
    }
    
    return folderPath + elementNames[selectionIndex];

}

}
