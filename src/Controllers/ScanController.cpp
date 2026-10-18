#include "ScanController.h"
#include <Models/ScanRemoteTarget.h>

namespace controllers {

ScanController::ScanController(interfaces::IDisplayView& display, interfaces::IInputProvider& input, ProductService& productService, 
                               RemoteService& remoteService, InfraredService& infraredService, ScanSelection& scanSelection)
    : display(display), input(input), productService(productService), scanSelection(scanSelection),
      remoteService(remoteService), infraredService(infraredService) {}

void ScanController::handleScanSelection() {
    std::vector<Product> products = productService.getManufacturerProducts(selectionContext.getCurrentSelectedManufacturer());
    std::vector<ScanRemoteTarget> targets;
    std::string favoriteName;
    std::string selectedProductName;

    for (const auto& product : products) {
        std::vector<Remote> remotes = remoteService.getRemotes(product);
        for (const auto& remote : remotes) {
            targets.push_back({product.name, remote});
        }
    }

    Remote selectedRemote = scanSelection.select(
        targets,
        selectionContext.getCurrentSelectedManufacturer().name,
        [&](const RemoteCommand& command, std::string manufacturerName) {
            (void)manufacturerName;
            return infraredService.sendRemoteCommandChecked(command);
        },
        favoriteName,
        selectedProductName,
        remoteService.getEmptyRemote()
    );

    if (selectedRemote != remoteService.getEmptyRemote() && !favoriteName.empty()) {
        remoteService.addFavoriteRemote(
            selectionContext.getCurrentSelectedManufacturer().name,
            selectedProductName,
            selectedRemote.fileName,
            favoriteName
        );
    }

    selectionContext.setCurrentManufacturerIndex(0);
    selectionContext.setIsManufacturerSelected(false);
    selectionContext.setIsModeSelected(false);
}

} // namespace controllers
