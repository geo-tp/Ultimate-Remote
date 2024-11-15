#include "ScanController.h"

namespace controllers {

ScanController::ScanController(CardputerView& display, CardputerInput& input, ProductService& productService, 
                               RemoteService& remoteService, InfraredService& infraredService, 
                               GlobalContext& globalContext, SelectionContext& selectionContext, ScanSelection& scanSelection)
    : display(display), input(input), productService(productService), remoteService(remoteService),
      infraredService(infraredService), globalContext(globalContext), selectionContext(selectionContext), scanSelection(scanSelection) {}

void ScanController::handleScanSelection() {
    std::vector<Product> products = productService.getManufacturerProducts(selectionContext.getCurrentSelectedManufacturer());
    std::string favoriteName;
    bool last = false;

    for (auto it = products.begin(); it != products.end(); ++it) {
        const auto& product = *it;
        last = (std::next(it) == products.end());

        std::vector<Remote> remotes = remoteService.getRemotes(product);
        
        Remote selectedRemote = scanSelection.select(
            remotes, 
            selectionContext.getCurrentSelectedManufacturer().name,
            [&](const RemoteCommand& command, std::string manufacturerName) { 
                infraredService.sendRemoteCommand(command, manufacturerName); 
            },
            favoriteName, 
            last, 
            remoteService.getEmptyRemote()
        );

        if (selectedRemote != remoteService.getEmptyRemote() && !favoriteName.empty()) {
            remoteService.addFavoriteRemote(
                selectionContext.getCurrentSelectedManufacturer().name, 
                product.name, 
                selectedRemote.fileName, 
                favoriteName
            );
            break;
        }
    }

    selectionContext.setCurrentManufacturerIndex(0);
    selectionContext.setIsManufacturerSelected(false);
    selectionContext.setIsModeSelected(false);
}

} // namespace controllers
