#ifndef APP_DISPATCHER_H
#define APP_DISPATCHER_H

#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Interfaces/IInfraredSender.h>
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
#include <Services/ManufacturerService.h>
#include <Controllers/InfoController.h>
#include <Controllers/SettingsController.h>
#include <Controllers/IrBuilderController.h>
#include <Interfaces/IAppPlatform.h>
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

using namespace controllers;
using namespace contexts;
using namespace enums;
using namespace repositories;
using namespace selections;
using namespace services;

namespace dispatchers {

class AppDispatcher {
public:
    AppDispatcher(interfaces::IDisplayView& display,
                  interfaces::IInputProvider& input,
                  interfaces::IInfraredSender& infraredSender,
                  interfaces::IAppPlatform& platform);
    
    void setup();
    void run();

private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    interfaces::IAppPlatform& platform;

    // Shared selection state used only for routing.
    SelectionContext& selectionContext;

    // Selections
    ModeSelection modeSelection;
    RemoteSelection remoteSelection;
    FileRemoteCommandSelection fileRemoteCommandSelection;
    RemoteCommandSelection remoteCommandSelection;
    ManufacturerSelection manufacturerSelection;
    ProductSelection productSelection;
    FilePathSelection filepathSelection;
    ScanSelection scanSelection;

    // Repositories
    ManufacturerRepository manufacturerRepository;
    ProductRepository productRepository;
    RemoteRepository remoteRepository;
    ProtocolRepository protocolRepository;

    // Services
    ManufacturerService manufacturerService;
    ProductService productService;
    RemoteService remoteService;
    FileService fileService;
    SdService sdService;
    LedService ledService;
    InfraredService infraredService;

    // Controllers
    ManufacturerController manufacturerController;
    ModeController modeController;
    RemoteController remoteController;
    ProductController productController;
    CommandController commandController;
    ScanController scanController;
    InfoController infoController;
    SettingsController settingsController;
    IrBuilderController irBuilderController;
};

} // namespace dispatchers

#endif // APP_DISPATCHER_H
