#ifndef FILE_REMOTE_COMMAND_SELECTION_H
#define FILE_REMOTE_COMMAND_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Models/FileRemoteCommand.h>

using namespace views;
using namespace inputs;
using namespace models;

namespace selections {

class FileRemoteCommandSelection {
public:
    FileRemoteCommandSelection(CardputerView& display, CardputerInput& input);
    FileRemoteCommand select(const std::vector<FileRemoteCommand>& commands, 
                        const std::string& remoteName, 
                        uint16_t& selectionIndex, 
                        FileRemoteCommand emptyRemoteCommand);
private:
    CardputerView& display;
    CardputerInput& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
