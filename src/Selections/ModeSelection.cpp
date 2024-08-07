#include "ModeSelection.h"

namespace selections {

ModeSelection::ModeSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), selectionIndex(0), lastIndex(-1) {}

SelectionMode ModeSelection::select() {
    display.displayTopBar("Ultimate Remote");
    char key = KEY_NONE;

    while (key != KEY_OK) {
        if (lastIndex != selectionIndex) {
            display.displaySelection(getSelectionModeStrings(), selectionIndex, getSelectionModeDescriptionStrings());
            lastIndex = selectionIndex;
        }

        key = input.handler();

        switch (key) {
            case KEY_ARROW_DOWN:
                selectionIndex = (selectionIndex < static_cast<uint8_t>(SelectionMode::COUNT) - 1) ? selectionIndex + 1 : 0;
                break;
            case KEY_ARROW_UP:
                selectionIndex = (selectionIndex > 0) ? selectionIndex - 1 : static_cast<uint8_t>(SelectionMode::COUNT) - 1;
                break;
            default:
                break;
        }
    }

    return static_cast<SelectionMode>(selectionIndex);
}


const std::string ModeSelection::getSelectionModeToString(SelectionMode mode) {
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

const std::string ModeSelection::getSelectionModeDescription(SelectionMode mode) {
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

const std::vector<std::string> ModeSelection::getSelectionModeStrings() {
    std::vector<std::string> modeStrings;
    for (int i = 0; i < static_cast<int>(SelectionMode::COUNT); ++i) {
        modeStrings.push_back(getSelectionModeToString(static_cast<SelectionMode>(i)));
    }
    return modeStrings;
}

const std::vector<std::string> ModeSelection::getSelectionModeDescriptionStrings() {
    std::vector<std::string> descriptionStrings;
    for (int i = 0; i < static_cast<int>(SelectionMode::COUNT); ++i) {
        descriptionStrings.push_back(getSelectionModeDescription(static_cast<SelectionMode>(i)));
    }
    return descriptionStrings;
}

}
