#include "CommandController.h"
#include <Platform/Compatibility/ArduinoCompat.h>

namespace controllers {

CommandController::CommandController(interfaces::IDisplayView& display, interfaces::IInputProvider& input, RemoteService& remoteService,
                                     ProductService& productService, InfraredService& infraredService,
                                     LedService& ledService, RemoteCommandSelection& remoteCommandSelection,
                                     FileRemoteCommandSelection& fileRemoteCommandSelection)
    : display(display), input(input), remoteService(remoteService), productService(productService),
      infraredService(infraredService), ledService(ledService),
      remoteCommandSelection(remoteCommandSelection), fileRemoteCommandSelection(fileRemoteCommandSelection) {}

void CommandController::handleRemoteCommandSelection() {
    std::vector<RemoteCommand> commands = remoteService.getRemoteCommands(selectionContext.getCurrentSelectedRemote());
    bool isFavoriteMode = selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES;
    uint16_t currentRemoteCommandIndex = selectionContext.getCurrentRemoteCommandIndex();



    while (true) {
        RemoteCommand command = remoteCommandSelection.select(
            commands,
            selectionContext.getCurrentSelectedRemote().fileName,
            selectionContext.getCurrentSelectedProduct().name,
            currentRemoteCommandIndex,
            remoteService.getUserFavoriteRemotes(),
            isFavoriteMode,
            [&](const std::string& filename, const std::string& favoriteName) {
                return remoteService.addFavoriteRemote(selectionContext.getCurrentSelectedManufacturer().name,
                                                       selectionContext.getCurrentSelectedProduct().name, filename, favoriteName);
            },
            [&](const std::string& favoriteName) {
                return remoteService.removeFavoriteRemote(favoriteName, selectionContext.getCurrentSelectedProduct().name);
            },
            remoteService.getEmptyRemoteCommand()
        );

        if (command == remoteService.getEmptyRemoteCommand()) {
            selectionContext.setIsRemoteSelected(false);
            selectionContext.setCurrentRemoteCommandIndex(0);
            break;
        } else {
            bool sent = infraredService.sendRemoteCommand(command, selectionContext.getCurrentSelectedManufacturer().name);
            selectionContext.setCurrentRemoteCommandIndex(currentRemoteCommandIndex);
            if (sent) ledService.blink();
            display.displayTransmissionFeedback(command.functionName, sent);
            delay(200);
        }
    }
}

void CommandController::handleFileRemoteCommandSelection() {
    uint16_t currentFileRemoteCommandIndex = selectionContext.getCurrentFileRemoteCommandIndex();

    while (true) {
        FileRemoteCommand command = fileRemoteCommandSelection.select(
            selectionContext.getCurrentSelectedFileRemote().commands,
            selectionContext.getCurrentSelectedFileRemote().fileName,
            currentFileRemoteCommandIndex,
            remoteService.getEmptyFileRemoteCommand()
        );

        if (command == remoteService.getEmptyFileRemoteCommand()) {
            selectionContext.setIsFileRemoteSelected(false);
            break;
        } else {
            bool sent = infraredService.sendFileRemoteCommand(command, selectionContext.getCurrentSelectedFileRemote().fileName);
            if (sent) ledService.blink();
            display.displayTransmissionFeedback(command.functionName, sent);
            delay(sent ? 200 : 700);
        }
    }
}

} // namespace controllers
