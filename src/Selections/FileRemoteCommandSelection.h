#ifndef FILE_REMOTE_COMMAND_SELECTION_H
#define FILE_REMOTE_COMMAND_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Models/FileRemoteCommand.h>

using namespace models;

namespace selections {

class FileRemoteCommandSelection {
public:
    FileRemoteCommandSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    FileRemoteCommand select(const std::vector<FileRemoteCommand>& commands, 
                        const std::string& remoteName, 
                        uint16_t& selectionIndex, 
                        FileRemoteCommand emptyRemoteCommand);
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
