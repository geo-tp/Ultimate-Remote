#ifndef MANUFACTURER_SELECTION_H
#define MANUFACTURER_SELECTION_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Models/Manufacturer.h>
#include <Contexts/GlobalContext.h>
#include <Utils/StringUtils.h>

using namespace contexts;
using namespace models;

namespace selections {

class ManufacturerSelection {
public:
    ManufacturerSelection(interfaces::IDisplayView& display, interfaces::IInputProvider& input);
    std::string select(const std::vector<std::string>& manufacturerNames, uint16_t& selectionIndex, bool scan);

private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    int16_t lastIndex;
};

}

#endif // MODE_SELECTION_H
