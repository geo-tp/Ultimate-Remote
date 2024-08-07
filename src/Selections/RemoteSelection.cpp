#include "RemoteSelection.h"

namespace selections {

RemoteSelection::RemoteSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), selectionIndex(0), lastIndex(-1) {}

Remote RemoteSelection::select(const std::vector<Remote>& remotes, const std::string& productName, uint16_t& selectionIndex, Remote emptyRemote) {
    char key = KEY_NONE;
    lastIndex = -1;
    std::vector<std::string> remoteNames = utils::StringUtils::extractFieldNames(remotes, "fileName");
    remoteNames = utils::StringUtils::addRemotesPrefixes(remoteNames);

    display.displayTopBar(productName, true);

    while (key != KEY_OK) {
        if (lastIndex != selectionIndex) {
            display.displaySelection(remoteNames, selectionIndex);
            lastIndex = selectionIndex;
        }

        key = input.handler();

        switch (key) {
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

}
