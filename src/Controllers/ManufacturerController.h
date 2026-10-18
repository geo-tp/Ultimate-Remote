#ifndef MANUFACTURER_CONTROLLER_H
#define MANUFACTURER_CONTROLLER_H

#include <Selections/ManufacturerSelection.h>
#include <Services/ManufacturerService.h>
#include <Contexts/SelectionContext.h>

using namespace selections;
using namespace services;
using namespace repositories;
using namespace contexts;

namespace controllers {

class ManufacturerController {
public:
    ManufacturerController(ManufacturerService& manufacturerService,
                           ManufacturerSelection& manufacturerSelection);

    void handleManufacturerSelection(bool scanMode = false);

private:
    ManufacturerService& manufacturerService;
    ManufacturerSelection& manufacturerSelection;
    SelectionContext& selectionContext = SelectionContext::getInstance();
};

} // namespace controllers

#endif // MANUFACTURER_CONTROLLER_H
