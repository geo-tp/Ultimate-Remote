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

namespace controllers {

class CommandController {
public:
    CommandController(views::CardputerView& display, inputs::CardputerInput& input, services::RemoteService& remoteService,
                      services::ProductService& productService, services::InfraredService& infraredService,
                      services::LedService& ledService, selections::RemoteCommandSelection& remoteCommandSelection,
                      selections::FileRemoteCommandSelection& fileRemoteCommandSelection);

    void handleRemoteCommandSelection();
    void handleFileRemoteCommandSelection();

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    services::RemoteService& remoteService;
    services::ProductService& productService;
    services::InfraredService& infraredService;
    services::LedService& ledService;
    selections::RemoteCommandSelection& remoteCommandSelection;
    selections::FileRemoteCommandSelection& fileRemoteCommandSelection;
    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    contexts::SelectionContext& selectionContext = contexts::SelectionContext::getInstance();
};

} // namespace controllers

#endif // COMMAND_CONTROLLER_H
