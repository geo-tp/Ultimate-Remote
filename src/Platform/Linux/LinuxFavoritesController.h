#pragma once
#ifndef ARDUINO
#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Contexts/SelectionContext.h>
#include <Services/RemoteService.h>
#include <Services/ProductService.h>
#include <Services/ManufacturerService.h>
#include <Controllers/CommandController.h>
#include <Platform/Linux/LinuxIrdbBrowser.h>

namespace platform::linux {
class LinuxFavoritesController {
public:
    LinuxFavoritesController(interfaces::IDisplayView& view, interfaces::IInputProvider& keys,
                        services::RemoteService& remotes, services::ProductService& products,
                        services::ManufacturerService& manufacturers, controllers::CommandController& commands,
                        platform::linux::LinuxIrdbBrowser& browser)
        : display(view), input(keys), remoteService(remotes), productService(products),
          manufacturerService(manufacturers), commandController(commands), irdbBrowser(browser),
          selectionContext(contexts::SelectionContext::getInstance()) {}
    void handle();
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    services::RemoteService& remoteService;
    services::ProductService& productService;
    services::ManufacturerService& manufacturerService;
    controllers::CommandController& commandController;
    platform::linux::LinuxIrdbBrowser& irdbBrowser;
    contexts::SelectionContext& selectionContext;
};
}
#endif
