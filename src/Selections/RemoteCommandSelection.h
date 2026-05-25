#ifndef REMOTE_COMMAND_SELECTION_H
#define REMOTE_COMMAND_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Models/RemoteCommand.h>
#include <Models/FavoriteRemote.h>

namespace selections {

class RemoteCommandSelection {
public:
    RemoteCommandSelection(views::CardputerView& display, inputs::CardputerInput& input);
    models::RemoteCommand select(const std::vector<models::RemoteCommand>& commands,
                        const std::string& remoteFileName,
                        const std::string& productName,
                        uint16_t& selectionIndex,
                        std::vector<models::FavoriteRemote> favoriteRemotes,
                        bool isFavoriteMode,
                        std::function<std::vector<models::FavoriteRemote>(const std::string&, const std::string&)> addFavorite,
                        std::function<std::vector<models::FavoriteRemote>(const std::string&)> deleteFavorite,
                        models::RemoteCommand emptyRemoteCommand);
private:
    ConfirmationSelection confirmationSelection;
    StringPromptSelection stringPromptSelection;
    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    views::CardputerView& display;
    inputs::CardputerInput& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
