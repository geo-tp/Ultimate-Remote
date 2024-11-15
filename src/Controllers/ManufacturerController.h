#ifndef MANUFACTURER_CONTROLLER_H
#define MANUFACTURER_CONTROLLER_H

#include <Selections/ManufacturerSelection.h>
#include <Repositories/ManufacturerRepository.h>
#include <Contexts/SelectionContext.h>

using namespace selections;
// using namespace services; // TODO:
using namespace repositories;
using namespace contexts;

namespace controllers {

class ManufacturerController {
public:
    ManufacturerController(ManufacturerRepository& manufacturerService,  // TODO:
                           SelectionContext& selectionContext,
                           ManufacturerSelection& manufacturerSelection);

    void handleManufacturerSelection(bool scanMode = false);

private:
    ManufacturerRepository& manufacturerService;
    ManufacturerSelection& manufacturerSelection;
    SelectionContext& selectionContext;
};

} // namespace controllers

#endif // MANUFACTURER_CONTROLLER_H
