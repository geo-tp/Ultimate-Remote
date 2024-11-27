#include "CommandController.h"

namespace controllers {

CommandController::CommandController(CardputerView& display, CardputerInput& input, RemoteService& remoteService,
                                     ProductService& productService, InfraredService& infraredService,
                                     LedService& ledService, RemoteCommandSelection& remoteCommandSelection,
                                     FileRemoteCommandSelection& fileRemoteCommandSelection)
    : display(display), input(input), remoteService(remoteService), productService(productService),
      infraredService(infraredService), ledService(ledService),
      remoteCommandSelection(remoteCommandSelection), fileRemoteCommandSelection(fileRemoteCommandSelection) {
        infraredService.begin();
      }

void CommandController::handleRemoteCommandSelection() {
    std::vector<RemoteCommand> commands = remoteService.getRemoteCommands(selectionContext.getCurrentSelectedRemote());
    bool isFavoriteMode = selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES;
    uint16_t currentRemoteCommandIndex = selectionContext.getCurrentRemoteCommandIndex();

    if (isFavoriteMode) {
        std::vector<FavoriteRemote> favoriteRemotes = remoteService.getAllFavoriteRemotes();
        selectionContext.setCurrentSelectedProduct(productService.setProductName(
            selectionContext.getCurrentSelectedProduct(), favoriteRemotes[selectionContext.getCurrentRemoteCommandIndex()].productName));
    }

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
            selectionContext.setCurrentRemoteIndex(0);
            break;
        } else {
            infraredService.sendRemoteCommand(command, selectionContext.getCurrentSelectedManufacturer().name);
            selectionContext.setCurrentRemoteIndex(currentRemoteCommandIndex);
            ledService.blink();
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
            infraredService.sendFileRemoteCommand(command, selectionContext.getCurrentSelectedFileRemote().fileName);
            ledService.blink();
        }
    }
}

} // namespace controllers
