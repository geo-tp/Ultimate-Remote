#ifndef REMOTE_COMMAND_SELECTION_H
#define REMOTE_COMMAND_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/StringPromptSelection.h>


using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

class RemoteCommandSelection {
public:
    RemoteCommandSelection(CardputerView& display, CardputerInput& input, GlobalContext& globalContext);
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
    GlobalContext& globalContext;
    CardputerView& display;
    CardputerInput& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
