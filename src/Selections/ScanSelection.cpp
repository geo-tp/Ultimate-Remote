#include "ScanSelection.h"
#include <Inputs/InputKeys.h>
#include <Platform/Compatibility/ArduinoCompat.h>

namespace selections {

ScanSelection::ScanSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input)
    : display(display), input(input), confirmationSelection(display, input), stringPromptSelection(display, input), lastIndex(-1) {}


Remote ScanSelection::select(const std::vector<ScanRemoteTarget>& targets,
                             const std::string manufacturerName, 
                             std::function<enums::InfraredSendResultEnum(const RemoteCommand&, std::string)> sendCommand,
                             std::string& favoriteName,
                             std::string& selectedProductName,
                             Remote emptyRemote) {

    char key = KEY_NONE;
    // The response window starts after transmission. Sony SIRC already sends
    // three frames (~135ms), so there is no need for an extra-long pause.
    constexpr unsigned long kResponseWindowMs = 180;

    display.displayTopBar("Remotes Scan ");
    display.displayScanSelection(manufacturerName);

    for (const auto& target : targets) {
        const Remote& remote = target.remote;
        display.displayScanRemote(target.productName + " / " + remote.fileName);
        for (size_t i = 0; i < remote.commandCount; ++i) {
            display.displayScanCommand(remote.commands[i].functionName);
            // Skip commands that cannot be transmitted (e.g. unsupported IRP protocols).
            // They must not be treated as commands that were actually tested.
            const auto result = sendCommand(remote.commands[i], manufacturerName);
            if (result == enums::InfraredSendResultEnum::Unsupported) continue;
            if (result == enums::InfraredSendResultEnum::TransmitFailed) {
                // Do not race through the brand at thousands of failed IR
                // commands/second. A hardware error means nothing was tested.
                display.displayMessage("IR TX failed", {"Check IR output / GPIO", "OK or Back to stop"});
                do {
                    key = input.handler();
                    delay(1);
                } while (key != KEY_OK && key != KEY_RETURN);
                return emptyRemote;
            }
            const unsigned long startTime = millis();
            while (millis() - startTime < kResponseWindowMs) {
                key = input.handler();
                if (key == KEY_FAVORITE) {
                    favoriteName = stringPromptSelection.select("Type a favorite name");
                    selectedProductName = target.productName;
                    return remote; // Retourner la remote selectionné
                }
                if (key == KEY_RETURN) {
                    return emptyRemote;
                }
                delay(1); // Do not busy-spin while waiting for a user reaction.
            }
        }
    }

    display.displayScanOver();
    while (key != KEY_OK && key != KEY_RETURN) {
        key = input.handler();
    }

    return emptyRemote;
}

}
