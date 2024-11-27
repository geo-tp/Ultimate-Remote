#ifndef COMMAND_CONTROLLER_H
#define COMMAND_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/RemoteService.h>
#include <Services/ProductService.h>
#include <Services/InfraredService.h>
#include <Services/LedService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Selections/RemoteCommandSelection.h>
#include <Selections/FileRemoteCommandSelection.h>

using namespace views;
using namespace inputs;
using namespace services;
using namespace contexts;
using namespace selections;

namespace controllers {

class CommandController {
public:
    CommandController(CardputerView& display, CardputerInput& input, RemoteService& remoteService,
                      ProductService& productService, InfraredService& infraredService,
                      LedService& ledService, RemoteCommandSelection& remoteCommandSelection,
                      FileRemoteCommandSelection& fileRemoteCommandSelection);

    void handleRemoteCommandSelection();
    void handleFileRemoteCommandSelection();

private:
    CardputerView& display;
    CardputerInput& input;
    RemoteService& remoteService;
    ProductService& productService;
    InfraredService& infraredService;
    LedService& ledService;
    RemoteCommandSelection& remoteCommandSelection; 
    FileRemoteCommandSelection& fileRemoteCommandSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();
};

} // namespace controllers

#endif // COMMAND_CONTROLLER_H
