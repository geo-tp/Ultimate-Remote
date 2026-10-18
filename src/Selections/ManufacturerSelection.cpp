#include "ManufacturerSelection.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedTextInput.h>
#include <Inputs/ScopedZBack.h>

namespace selections {

ManufacturerSelection::ManufacturerSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input)
    : display(display), input(input), lastIndex(-1) {}

std::string ManufacturerSelection::select(const std::vector<std::string>& manufacturerNames, uint16_t& selectionIndex, bool scan) {
    inputs::ScopedZBack zBack(input);
    inputs::ScopedTextInput textInput(input);
    char key = KEY_NONE;
    lastIndex = -1;
    std::vector<std::string> filteredManufacturerNames = manufacturerNames;
    std::string searchQuery = "";
    std::string upperSearchQuery;
    std::string upperManufacturerName;

    (void)scan;
    if (selectionIndex >= manufacturerNames.size()) selectionIndex = 0;

    display.displayTopBar("Type to filter", true, true); // searchbar
    display.displaySelection(filteredManufacturerNames, selectionIndex);

    while (key != KEY_OK) {
        key = input.handler();

        switch (key) {
            case KEY_NONE:
                break;

            case KEY_OK:
                if (filteredManufacturerNames.empty()) key = KEY_NONE;
                break;

            case KEY_ARROW_DOWN:
                if (filteredManufacturerNames.empty()) {
                    break;
                }
                if (selectionIndex < filteredManufacturerNames.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (filteredManufacturerNames.empty()) {
                    break;
                }
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = filteredManufacturerNames.size() - 1;
                }
                break;
            case KEY_RETURN:
                return ""; // empty return

            default:
                if (key == KEY_DEL) {
                    if (searchQuery.length() > 0) {
                        searchQuery.pop_back();
                    }
                } else if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                           (key >= '0' && key <= '9') ||
                           ((key == ' ' || key == KEY_FILTER_SPACE) && !searchQuery.empty()) ||
                           key == '-' || key == '_') {
                    searchQuery += key == KEY_FILTER_SPACE ? ' ' : key;
                    selectionIndex = 0; // Reset the selection after each char entry
                } else {
                    break;
                }

                if (searchQuery.empty() == true) {
                    filteredManufacturerNames = manufacturerNames; // revert to full list
                } else {
                    filteredManufacturerNames = {};
                    selectionIndex = 0;
                    upperSearchQuery = utils::StringUtils::toUpperCase(searchQuery);

                    // Filter the list of manufacturers based on the search bar, compare uppercase
                    for (const auto& name : manufacturerNames) {
                        upperManufacturerName = utils::StringUtils::toUpperCase(name);
                        if (upperManufacturerName.find(upperSearchQuery) != std::string::npos) {
                            filteredManufacturerNames.push_back(name);
                        }
                    }
                }
                if (!filteredManufacturerNames.empty() && selectionIndex >= filteredManufacturerNames.size()) {
                    selectionIndex = 0;
                }
                break;
        }

        if (key != KEY_NONE) {
            // Update screen
            display.displayTopBar(searchQuery.empty() ? "Type to filter" : searchQuery, true, true);
            display.displaySelection(filteredManufacturerNames, selectionIndex);
        }
    }

    if (filteredManufacturerNames.empty()) {
        return "";
    }

    return filteredManufacturerNames[selectionIndex];
}

}
