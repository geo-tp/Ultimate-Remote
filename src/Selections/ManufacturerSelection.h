#ifndef MANUFACTURER_SELECTION_H
#define MANUFACTURER_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Models/Manufacturer.h>
#include <Contexts/GlobalContext.h>
#include <Utils/StringUtils.h>

namespace selections {

class ManufacturerSelection {
public:
    ManufacturerSelection(views::CardputerView& display, inputs::CardputerInput& input);
    std::string select(const std::vector<std::string>& manufacturerNames, uint16_t& selectionIndex, bool scan);

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
