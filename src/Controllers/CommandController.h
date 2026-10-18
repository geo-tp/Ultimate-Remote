#ifndef COMMAND_CONTROLLER_H
#define COMMAND_CONTROLLER_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Services/RemoteService.h>
#include <Services/ProductService.h>
#include <Services/InfraredService.h>
#include <Services/LedService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Selections/RemoteCommandSelection.h>
#include <Selections/FileRemoteCommandSelection.h>

using namespace services;
using namespace contexts;
using namespace selections;

namespace controllers {

class CommandController {
public:
    CommandController(interfaces::IDisplayView& display, interfaces::IInputProvider& input, RemoteService& remoteService,
                      ProductService& productService, InfraredService& infraredService,
                      LedService& ledService, RemoteCommandSelection& remoteCommandSelection,
                      FileRemoteCommandSelection& fileRemoteCommandSelection);

        void handleRemoteCommandSelection();
        void handleFileRemoteCommandSelection();

private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
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
