#ifndef FILEPATH_SELECTION_H
#define FILEPATH_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Utils/StringUtils.h>

using namespace views;
using namespace inputs;
using namespace contexts;
using namespace utils;

namespace selections {

class FilePathSelection {
public:
    FilePathSelection(CardputerView& display, CardputerInput& input);
    std::string select(const std::vector<std::string>& elementNames, std::string folderpath, uint16_t& selectionIndex, bool firstRun);
private:
    GlobalContext& context = GlobalContext::getInstance();
    CardputerView& display;
    CardputerInput& input;
    uint8_t selectionIndex;
    int16_t lastIndex;
    bool firstRun = true;

    void handleFirstRun(bool firstRun);
};

}

#endif // MODE_SELECTION_H
