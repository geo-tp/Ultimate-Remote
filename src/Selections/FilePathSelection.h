#ifndef FILEPATH_SELECTION_H
#define FILEPATH_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Utils/StringUtils.h>

namespace selections {

class FilePathSelection {
public:
    FilePathSelection(views::CardputerView& display, inputs::CardputerInput& input);
    std::string select(const std::vector<std::string>& elementNames, std::string folderpath, uint16_t& selectionIndex, bool firstRun);
private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    int16_t lastIndex;

    void handleFirstRun(bool firstRun);
};

}

#endif // MODE_SELECTION_H
