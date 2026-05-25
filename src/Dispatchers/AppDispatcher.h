#ifndef APP_DISPATCHER_H
#define APP_DISPATCHER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Controllers/ManufacturerController.h>
#include <Controllers/RemoteController.h>
#include <Controllers/ScanController.h>
#include <Controllers/ProductController.h>
#include <Controllers/ModeController.h>
#include <Controllers/CommandController.h>
#include <Repositories/ManufacturerRepository.h>
#include <Repositories/ProtocolRepository.h>
#include <Repositories/RemoteRepository.h>
#include <Repositories/ProductRepository.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Enums/SelectionModeEnum.h>
#include <Selections/ManufacturerSelection.h>
#include <Selections/RemoteSelection.h>
#include <Selections/FileRemoteCommandSelection.h>
#include <Selections/ScanSelection.h>
#include <Selections/FilePathSelection.h>
#include <Selections/RemoteCommandSelection.h>
#include <Selections/ModeSelection.h>
#include <Selections/ProductSelection.h>
#include <Services/RemoteService.h>
#include <Services/FileService.h>
#include <Services/SdService.h>
#include <Services/LedService.h>
#include <Services/InfraredService.h>

namespace dispatchers {

class AppDispatcher {
public:
    AppDispatcher(views::CardputerView& display, inputs::CardputerInput& input);

    void setup();
    void run();

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;

    // Singleton instances for context management
    contexts::GlobalContext& globalContext;
    contexts::SelectionContext& selectionContext;

    // Selections
    selections::ModeSelection modeSelection;
    selections::RemoteSelection remoteSelection;
    selections::FileRemoteCommandSelection fileRemoteCommandSelection;
    selections::RemoteCommandSelection remoteCommandSelection;
    selections::ManufacturerSelection manufacturerSelection;
    selections::ProductSelection productSelection;
    selections::FilePathSelection filepathSelection;
    selections::ScanSelection scanSelection;

    // Repositories
    repositories::ManufacturerRepository manufacturerRepository;
    repositories::ProductRepository productRepository;
    repositories::RemoteRepository remoteRepository;
    repositories::ProtocolRepository protocolRepository;

    // Services
    repositories::ManufacturerRepository manufacturerService;
    services::ProductService productService;
    services::RemoteService remoteService;
    services::FileService fileService;
    services::SdService sdService;
    services::LedService ledService;
    services::InfraredService infraredService;

    // Controllers
    controllers::ManufacturerController manufacturerController;
    controllers::ModeController modeController;
    controllers::RemoteController remoteController;
    controllers::ProductController productController;
    controllers::CommandController commandController;
    controllers::ScanController scanController;
};

} // namespace dispatchers

#endif // APP_DISPATCHER_H
