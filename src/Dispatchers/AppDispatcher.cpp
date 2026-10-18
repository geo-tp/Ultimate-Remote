#include "AppDispatcher.h"
#include <Platform/AppPlatformDependencies.h>

namespace dispatchers {
AppDispatcher::AppDispatcher(interfaces::IDisplayView& display,
                             interfaces::IInputProvider& input,
                             interfaces::IInfraredSender& infraredSender,
                             interfaces::IAppPlatform& appPlatform)
    : display(display), input(input), platform(appPlatform),
      selectionContext(SelectionContext::getInstance()),
      modeSelection(display, input, appPlatform), remoteSelection(display, input),
      fileRemoteCommandSelection(display, input), remoteCommandSelection(display, input),
      manufacturerSelection(display, input), productSelection(display, input),
      filepathSelection(display, input), scanSelection(display, input),
      manufacturerRepository(), productRepository(), remoteRepository(appPlatform.favoriteStorage()), protocolRepository(),
      manufacturerService(manufacturerRepository), productService(productRepository),
      remoteService(remoteRepository), fileService(protocolRepository), sdService(),
      ledService(), infraredService(protocolRepository, infraredSender),
      manufacturerController(manufacturerService, manufacturerSelection),
      modeController(display, input, modeSelection),
      remoteController(display, input, remoteService, productService, manufacturerService,
                       fileService, sdService, remoteSelection, filepathSelection),
      productController(display, input, productService, productSelection),
      commandController(display, input, remoteService, productService, infraredService, ledService,
                        remoteCommandSelection, fileRemoteCommandSelection),
      scanController(display, input, productService, remoteService, infraredService, scanSelection),
      infoController(display, input, manufacturerRepository),
      settingsController(display, input, infraredService, appPlatform),
      irBuilderController(display, input, infraredService, ledService)
{
    platform.attach({display, input, remoteController, commandController,
                     fileService, remoteService, productService, manufacturerService});
}

void AppDispatcher::setup() {
    display.initialise();
    infraredService.begin();
}

void AppDispatcher::run() {
    while (true) {
        if (!selectionContext.getIsModeSelected()) {
            modeController.handleModeSelection();
        }
        else if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::SETTINGS) {
            settingsController.handle();
        }
        else if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::INFO) {
            infoController.handle();
        }
        else if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES &&
                 !selectionContext.getIsRemoteSelected()) {
            platform.openFavorites();
        }
        else if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::IR_BUILDER) {
            irBuilderController.handle();
        }
        else if (!selectionContext.getIsFileRemoteSelected() && selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FILES) {
            platform.openFiles();
        }
        else if (selectionContext.getIsFileRemoteSelected() && selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FILES) {
            commandController.handleFileRemoteCommandSelection();
        }
        else if (!selectionContext.getIsManufacturerSelected() && selectionContext.getCurrentSelectedMode() != SelectionModeEnum::FAVORITES) {
            manufacturerController.handleManufacturerSelection(selectionContext.getCurrentSelectedMode() == SelectionModeEnum::SCAN);
        }
        else if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::SCAN) {
            scanController.handleScanSelection();
        }
        else if (selectionContext.getIsManufacturerSelected() && selectionContext.getCurrentSelectedMode() == SelectionModeEnum::ALL_REMOTES) {
            if (!selectionContext.getIsRemoteSelected()) {
                remoteController.handleRemoteSelection();
            } else {
                commandController.handleRemoteCommandSelection();
            }
        }
        else if (!selectionContext.getIsProductSelected() && selectionContext.getCurrentSelectedMode() != SelectionModeEnum::SCAN) {
            productController.handleProductSelection();
        }
        else if (!selectionContext.getIsRemoteSelected()) {
            remoteController.handleRemoteSelection();
        }
        else if (selectionContext.getIsRemoteSelected()) {
            commandController.handleRemoteCommandSelection();
        }
    }
}

} // namespace dispatchers
