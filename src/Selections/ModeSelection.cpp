#include "ModeSelection.h"

namespace selections {

ModeSelection::ModeSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

SelectionModeEnum ModeSelection::select() {
    display.displayTopBar(globalContext.getAppName());
    char key = KEY_NONE;
    bool firstRender = true;

    while (key != KEY_OK) {
        if (lastIndex != selectionIndex || firstRender) {
            display.displaySelection(getSelectionModeStrings(), selectionIndex, getSelectionModeDescriptionStrings());
            lastIndex = selectionIndex;
            firstRender = false;
        }

        key = input.handler();

        switch (key) {
            case KEY_ARROW_DOWN:
                selectionIndex = (selectionIndex < static_cast<uint8_t>(SelectionModeEnum::COUNT) - 1) ? selectionIndex + 1 : 0;
                break;
            case KEY_ARROW_UP:
                selectionIndex = (selectionIndex > 0) ? selectionIndex - 1 : static_cast<uint8_t>(SelectionModeEnum::COUNT) - 1;
                break;
            default:
                break;
        }
    }

    return static_cast<SelectionModeEnum>(selectionIndex);
}


const std::string ModeSelection::getSelectionModeToString(SelectionModeEnum mode) {
    switch (mode) {
        case SelectionModeEnum::SCAN:
            return "SCANNER";
        case SelectionModeEnum::FAVORITES:
            return "FAVORITES";
        case SelectionModeEnum::FILES:
            return "READ FILES";
        case SelectionModeEnum::ALL_REMOTES:
            return "ALL REMOTES";
        default:
            return "UNKNOWN";
    }
}

const std::string ModeSelection::getSelectionModeDescription(SelectionModeEnum mode) {
    switch (mode) {
        case SelectionModeEnum::SCAN:
            return "   search for remotes";
        case SelectionModeEnum::FAVORITES:
            return "saved remotes";
        case SelectionModeEnum::FILES:
            return "from sd card";
        case SelectionModeEnum::ALL_REMOTES:
            return "  available";
        default:
            return "UNKNOWN";
    }
}

const std::vector<std::string> ModeSelection::getSelectionModeStrings() {
    std::vector<std::string> modeStrings;
    for (int i = 0; i < static_cast<int>(SelectionModeEnum::COUNT); ++i) {
        modeStrings.push_back(getSelectionModeToString(static_cast<SelectionModeEnum>(i)));
    }
    return modeStrings;
}

const std::vector<std::string> ModeSelection::getSelectionModeDescriptionStrings() {
    std::vector<std::string> descriptionStrings;
    for (int i = 0; i < static_cast<int>(SelectionModeEnum::COUNT); ++i) {
        descriptionStrings.push_back(getSelectionModeDescription(static_cast<SelectionModeEnum>(i)));
    }
    return descriptionStrings;
}

}
