#ifndef FILE_REMOTE_COMMAND_SELECTION_H
#define FILE_REMOTE_COMMAND_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Models/FileRemoteCommand.h>

namespace selections {

class FileRemoteCommandSelection {
public:
    FileRemoteCommandSelection(views::CardputerView& display, inputs::CardputerInput& input);
    models::FileRemoteCommand select(const std::vector<models::FileRemoteCommand>& commands,
                        const std::string& remoteName,
                        uint16_t& selectionIndex,
                        models::FileRemoteCommand emptyRemoteCommand);
private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
