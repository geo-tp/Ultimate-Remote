#ifndef SCAN_CONTROLLER_H
#define SCAN_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/ProductService.h>
#include <Services/RemoteService.h>
#include <Services/InfraredService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Selections/ScanSelection.h>
#include <Models/Remote.h>
#include <Models/Product.h>

namespace controllers {

class ScanController {
public:
    ScanController(views::CardputerView& display, inputs::CardputerInput& input, services::ProductService& productService,
                   services::RemoteService& remoteService, services::InfraredService& infraredService,
                   selections::ScanSelection& scanSelection);

    void handleScanSelection();

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    services::ProductService& productService;
    selections::ScanSelection& scanSelection;
    services::RemoteService& remoteService;
    services::InfraredService& infraredService;
    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    contexts::SelectionContext& selectionContext = contexts::SelectionContext::getInstance();
};

} // namespace controllers

#endif // SCAN_CONTROLLER_H
