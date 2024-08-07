#ifndef MANUFACTURER_SELECTION_H
#define MANUFACTURER_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Models/Manufacturer.h>
#include <Contexts/GlobalContext.h>
#include <Utils/StringUtils.h>

using namespace views;
using namespace inputs;
using namespace contexts;
using namespace models;

namespace selections {

class ManufacturerSelection {
public:
    ManufacturerSelection(CardputerView& display, CardputerInput& input);
    std::string select(const std::vector<std::string>& manufacturerNames, uint16_t& selectionIndex, bool scan);

private:
    GlobalContext& context = GlobalContext::getInstance();
    CardputerView& display;
    CardputerInput& input;
    uint8_t selectionIndex;
    int8_t lastIndex;
};

}

#endif // MODE_SELECTION_H
