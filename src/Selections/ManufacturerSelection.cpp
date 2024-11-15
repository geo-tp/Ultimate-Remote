#include "ManufacturerSelection.h"

namespace selections {

ManufacturerSelection::ManufacturerSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), lastIndex(-1) {}

std::string ManufacturerSelection::select(const std::vector<std::string>& manufacturerNames, uint16_t& selectionIndex, bool scan) {
    char key = KEY_NONE;
    lastIndex = -1;
    std::vector<std::string> filteredManufacturerNames = manufacturerNames;
    std::string searchQuery = "";
    std::string upperSearchQuery;
    std::string upperManufacturerName;

    if (scan) {
        display.displayScanInfo();
        while (key != KEY_OK) {
            key = input.handler();
        }

        key = KEY_NONE;
    }

    display.displayTopBar(searchQuery, true, true); // searchbar
    display.displaySelection(filteredManufacturerNames, selectionIndex);

    while (key!= KEY_OK) {
        key = input.handler();

        switch (key) {
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
                if (key == KEY_DEL) {
                    if (searchQuery.length() > 0) {
                        searchQuery.pop_back();
                    }
                } else {
                    searchQuery += key;
                    selectionIndex = 0; // Reset the selection after each char entry
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
                break;
        }

        if (key != KEY_NONE) {
            // Update screen
            display.displayTopBar(searchQuery, true, true);
            display.displaySelection(filteredManufacturerNames, selectionIndex);
        }
    }

    return filteredManufacturerNames[selectionIndex];
}

}