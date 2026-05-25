#include "ManufacturerController.h"

namespace controllers {

ManufacturerController::ManufacturerController(ManufacturerRepository& manufacturerService, 
                                               ManufacturerSelection& manufacturerSelection)
    : manufacturerService(manufacturerService), 
      manufacturerSelection(manufacturerSelection) {}

void ManufacturerController::handleManufacturerSelection(bool scanMode) {
    std::vector<std::string> manufacturerNames = ManufacturerRepository::getAllManufacturerNames();
    uint16_t manufacturerIndex = selectionContext.getCurrentManufacturerIndex();
    std::string selectedManufacturerName = manufacturerSelection.select(manufacturerNames, manufacturerIndex, scanMode);

    if (!selectedManufacturerName.empty()) {
        Manufacturer selectedManufacturer = ManufacturerRepository::getManufacturerByName(selectedManufacturerName);
        selectionContext.setCurrentSelectedManufacturer(selectedManufacturer);
        selectionContext.setIsManufacturerSelected(true);
        selectionContext.setCurrentManufacturerIndex(manufacturerIndex);
    } else {
        selectionContext.setIsManufacturerSelected(false);
        selectionContext.setIsModeSelected(false);
        selectionContext.setCurrentManufacturerIndex(0);
    }
}

} // namespace controllers
