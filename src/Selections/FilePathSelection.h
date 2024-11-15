#ifndef FILEPATH_SELECTION_H
#define FILEPATH_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Utils/StringUtils.h>

using namespace views;
using namespace inputs;
using namespace utils;

namespace selections {

class FilePathSelection {
public:
    FilePathSelection(CardputerView& display, CardputerInput& input);
    std::string select(const std::vector<std::string>& elementNames, std::string folderpath, uint16_t& selectionIndex, bool firstRun);
private:
    CardputerView& display;
    CardputerInput& input;
    int16_t lastIndex;

    void handleFirstRun(bool firstRun);
};

}

#endif // MODE_SELECTION_H
