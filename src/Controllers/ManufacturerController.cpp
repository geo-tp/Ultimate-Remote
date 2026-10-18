#include "ManufacturerController.h"

namespace controllers {

ManufacturerController::ManufacturerController(ManufacturerService& manufacturerService,
                                               ManufacturerSelection& manufacturerSelection)
    : manufacturerService(manufacturerService), 
      manufacturerSelection(manufacturerSelection) {}

void ManufacturerController::handleManufacturerSelection(bool scanMode) {
    std::vector<std::string> manufacturerNames = manufacturerService.getAllManufacturerNames();
    uint16_t manufacturerIndex = selectionContext.getCurrentManufacturerIndex();
    std::string selectedManufacturerName = manufacturerSelection.select(manufacturerNames, manufacturerIndex, scanMode);

    if (!selectedManufacturerName.empty()) {
        Manufacturer selectedManufacturer = manufacturerService.getManufacturer(selectedManufacturerName);
        selectionContext.setCurrentSelectedManufacturer(selectedManufacturer);
        selectionContext.setIsManufacturerSelected(true);
        selectionContext.setCurrentManufacturerIndex(manufacturerIndex);
        selectionContext.setIsProductSelected(false);
        selectionContext.setIsRemoteSelected(false);
        selectionContext.setCurrentProductIndex(0);
        selectionContext.setCurrentRemoteIndex(0);
        selectionContext.setCurrentRemoteCommandIndex(0);
    } else {
        selectionContext.setIsManufacturerSelected(false);
        selectionContext.setIsModeSelected(false);
        selectionContext.setCurrentManufacturerIndex(0);
    }
}

} // namespace controllers
