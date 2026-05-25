#ifndef MANUFACTURER_CONTROLLER_H
#define MANUFACTURER_CONTROLLER_H

#include <Selections/ManufacturerSelection.h>
#include <Repositories/ManufacturerRepository.h>
#include <Contexts/SelectionContext.h>

namespace controllers {

class ManufacturerController {
public:
    ManufacturerController(repositories::ManufacturerRepository& manufacturerService,
                           selections::ManufacturerSelection& manufacturerSelection);

    void handleManufacturerSelection(bool scanMode = false);

private:
    repositories::ManufacturerRepository& manufacturerService;
    selections::ManufacturerSelection& manufacturerSelection;
    contexts::SelectionContext& selectionContext = contexts::SelectionContext::getInstance();};

} // namespace controllers

#endif // MANUFACTURER_CONTROLLER_H
