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

using namespace views;
using namespace inputs;
using namespace services;
using namespace contexts;
using namespace selections;
using namespace models;

namespace controllers {

class ScanController {
public:
    ScanController(CardputerView& display, CardputerInput& input, ProductService& productService, 
                   RemoteService& remoteService, InfraredService& infraredService, 
                   GlobalContext& globalContext, SelectionContext& selectionContext, ScanSelection& ScanSelection);

    void handleScanSelection();

private:
    CardputerView& display;
    CardputerInput& input;
    ProductService& productService;
    ScanSelection& scanSelection;
    RemoteService& remoteService;
    InfraredService& infraredService;
    GlobalContext& globalContext;
    SelectionContext& selectionContext;
};

} // namespace controllers

#endif // SCAN_CONTROLLER_H
