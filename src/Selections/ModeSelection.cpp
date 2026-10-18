#include "ModeSelection.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedHomeMenuInput.h>

#include <algorithm>

namespace selections {

ModeSelection::ModeSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input,
                             interfaces::IAppPlatform& platform)
    : display(display), input(input), platform(platform), selectionIndex(0), lastIndex(-1) {}

SelectionModeEnum ModeSelection::select() {
    inputs::ScopedHomeMenuInput homeInput(input);
    std::vector<SelectionModeEnum> modes;
    std::vector<std::string> labels;
    std::vector<std::string> descriptions;
    for (int i = 0; i < static_cast<int>(SelectionModeEnum::COUNT); ++i) {
        const auto mode = static_cast<SelectionModeEnum>(i);
        if (!platform.supportsMode(mode)) continue;
        modes.push_back(mode);
        labels.push_back(getSelectionModeToString(mode));
        descriptions.push_back(getSelectionModeDescription(mode));
    }
    if (modes.empty()) return SelectionModeEnum::ALL_REMOTES;

    // SelectionIndex addresses only modes available on this platform.
    selectionIndex = std::min<size_t>(selectionIndex, modes.size() - 1);
    display.displayTopBar(globalContext.getAppName());
    char key = KEY_NONE;
    bool firstRender = true;
    int lastHoldStep = -1;

    while (key != KEY_OK) {
        if (lastIndex != selectionIndex || firstRender) {
            display.displayHomeCarousel(labels, selectionIndex, descriptions);
            lastIndex = selectionIndex;
            firstRender = false;
        }

        key = input.handler();
        const int held = input.exitHoldMilliseconds();
        if (held >= 0) {
            if (held / 50 != lastHoldStep) {
                display.displayExitHold(held);
                lastHoldStep = held / 50;
            }
            key = KEY_NONE;
            continue;
        }
        if (lastHoldStep >= 0) {
            firstRender = true;
            lastHoldStep = -1;
            key = KEY_NONE;
            continue;
        }

        const bool letters = input.homeMenuUsesLetterNavigation();
        if (key == KEY_ARROW_RIGHT || (!letters && key == KEY_ARROW_DOWN) ||
            (letters && (key == 'c' || key == 'C'))) {
            selectionIndex = (selectionIndex + 1) % modes.size();
        } else if (key == KEY_ARROW_LEFT || (!letters && key == KEY_ARROW_UP) ||
                   (letters && (key == 'z' || key == 'Z'))) {
            selectionIndex = selectionIndex ? selectionIndex - 1 : modes.size() - 1;
        }
    }

    return modes[selectionIndex];
}


const std::string ModeSelection::getSelectionModeToString(SelectionModeEnum mode) {
    switch (mode) {
        case SelectionModeEnum::ALL_REMOTES:
            return "REMOTES";
        case SelectionModeEnum::SCAN:
            return "SCANNER";
        case SelectionModeEnum::FAVORITES:
            return "FAVORITES";
        case SelectionModeEnum::FILES:
            return platform.fileMenuName();
        case SelectionModeEnum::IR_BUILDER:
            return "IR BUILDER";
        case SelectionModeEnum::SETTINGS:
            return "SETTINGS";
        case SelectionModeEnum::INFO:
            return "INFO";
        default:
            return "UNKNOWN";
    }
}

const std::string ModeSelection::getSelectionModeDescription(SelectionModeEnum mode) {
    switch (mode) {
        case SelectionModeEnum::ALL_REMOTES:
            return "Preloaded offline remotes";
        case SelectionModeEnum::SCAN:
            return "   search for remotes";
        case SelectionModeEnum::FAVORITES:
            return "saved remotes";
        case SelectionModeEnum::FILES:
            return platform.fileMenuDescription();
        case SelectionModeEnum::IR_BUILDER:
            return "Custom protocol and IR codes";
        case SelectionModeEnum::SETTINGS:
            return "IR transmitter GPIO";
        case SelectionModeEnum::INFO:
            return "About and quick guide";
        default:
            return "UNKNOWN";
    }
}

const std::vector<std::string> ModeSelection::getSelectionModeStrings() {
    std::vector<std::string> modeStrings;
    for (int i = 0; i < static_cast<int>(SelectionModeEnum::COUNT); ++i) {
        const auto mode = static_cast<SelectionModeEnum>(i);
        if (platform.supportsMode(mode)) modeStrings.push_back(getSelectionModeToString(mode));
    }
    return modeStrings;
}

const std::vector<std::string> ModeSelection::getSelectionModeDescriptionStrings() {
    std::vector<std::string> descriptionStrings;
    for (int i = 0; i < static_cast<int>(SelectionModeEnum::COUNT); ++i) {
        const auto mode = static_cast<SelectionModeEnum>(i);
        if (platform.supportsMode(mode)) descriptionStrings.push_back(getSelectionModeDescription(mode));
    }
    return descriptionStrings;
}

}
