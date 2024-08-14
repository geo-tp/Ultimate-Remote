#ifndef FILE_REMOTE_COMMAND_SELECTION_H
#define FILE_REMOTE_COMMAND_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/StringPromptSelection.h>
#include <Models/FileRemoteCommand.h>

using namespace views;
using namespace inputs;
using namespace contexts;
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
    GlobalContext& context = GlobalContext::getInstance();
    CardputerView& display;
    CardputerInput& input;
    uint16_t selectionIndex;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
