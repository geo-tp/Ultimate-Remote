#include "ScanSelection.h"

namespace selections {

ScanSelection::ScanSelection(CardputerView& display, CardputerInput& input, GlobalContext& globalContext)
    : display(display), input(input), globalContext(globalContext), confirmationSelection(display, input), stringPromptSelection(display, input, globalContext), lastIndex(-1) {}


Remote ScanSelection::select(const std::vector<Remote>& remotes, 
                             const std::string manufacturerName, 
                             std::function<void(const RemoteCommand&, std::string)> sendCommand, 
                             std::string& favoriteName, bool last, Remote emptyRemote) {

    char key = KEY_NONE;
    const int intervalMs =  180; // pour input utilisateur

    display.displayTopBar("Remotes Scan ");
    display.displayScanSelection(manufacturerName);

    for (const auto& remote : remotes) {
        display.displayScanRemote(remote.fileName);
        for (size_t i = 0; i < remote.commandCount; ++i) {
            sendCommand(remote.commands[i], manufacturerName);
            display.displayScanCommand(remote.commands[i].functionName);
            // Vérifier l'input utilisateur
            unsigned long startTime = millis();
            while (millis() - startTime < intervalMs) {
                key = input.handler();
                if (key == KEY_FAVORITE) {
                    favoriteName = stringPromptSelection.select("Type a favorite name");
                    return remote; // Retourner la remote selectionné
                }
            }
        }
    }

    // Last element, we display the end of the scan
    if (last) {
        display.displayScanOver();
        while (key != KEY_OK) {
            key = input.handler();
        }
    }

    return emptyRemote;
}

}