#ifndef FILEPATH_SELECTION_H
#define FILEPATH_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Utils/StringUtils.h>

using namespace utils;

namespace selections {

class FilePathSelection {
public:
    FilePathSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    std::string select(const std::vector<std::string>& elementNames, std::string folderpath, uint16_t& selectionIndex, bool firstRun);
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    int16_t lastIndex;

    void handleFirstRun(bool firstRun);
};

}

#endif // MODE_SELECTION_H
