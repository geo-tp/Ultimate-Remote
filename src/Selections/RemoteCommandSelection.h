#ifndef REMOTE_COMMAND_SELECTION_H
#define REMOTE_COMMAND_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Models/RemoteCommand.h>
#include <Models/FavoriteRemote.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <functional>


using namespace contexts;
using namespace models;

namespace selections {

class RemoteCommandSelection {
public:
    RemoteCommandSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    RemoteCommand select(const std::vector<RemoteCommand>& commands, 
                        const std::string& remoteFileName, 
                        const std::string& productName, 
                        uint16_t& selectionIndex, 
                        std::vector<FavoriteRemote> favoriteRemotes,
                        bool isFavoriteMode,
                        std::function<std::vector<FavoriteRemote>(const std::string&, const std::string&)> addFavorite, 
                        std::function<std::vector<FavoriteRemote>(const std::string&)> deleteFavorite,
                        RemoteCommand emptyRemoteCommand);
private:
    ConfirmationSelection confirmationSelection;
    StringPromptSelection stringPromptSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
