#include "AppDispatcher.h"

namespace dispatchers {

AppDispatcher::AppDispatcher(CardputerView& display, CardputerInput& input)
    : display(display), 
      input(input), 
      
      // Contexts
      globalContext(GlobalContext::getInstance()),
      selectionContext(SelectionContext::getInstance()),

      // Selections
      modeSelection(display, input, globalContext),
      remoteSelection(display, input),
      filepathSelection(display, input),
      remoteCommandSelection(display, input, globalContext),
      fileRemoteCommandSelection(display, input),
      productSelection(display, input),
      manufacturerSelection(display, input),
      scanSelection(display, input, globalContext),
      
      // Repositories
      manufacturerRepository(),
      productRepository(),
      remoteRepository(),
      protocolRepository(),
      
      // Services
      manufacturerService(), // TODO: Resolve this to use ManufacturerService instead of Repo
      productService(productRepository),
      remoteService(remoteRepository),
      fileService(protocolRepository),
      sdService(globalContext),
      infraredService(globalContext, protocolRepository),
      
      // Controllers
      modeController(display, input, globalContext, selectionContext, modeSelection),

      remoteController(display, input, remoteService, productService, manufacturerService, 
                       fileService, sdService, globalContext, selectionContext, remoteSelection, filepathSelection),
                       
      productController(display, input, productService, globalContext, selectionContext, productSelection),

      commandController(display, input, remoteService, productService, infraredService, ledService, globalContext, 
                        selectionContext, remoteCommandSelection, fileRemoteCommandSelection),

      scanController(display, input, productService, remoteService, infraredService, globalContext, selectionContext, scanSelection),

      manufacturerController(manufacturerService, selectionContext, manufacturerSelection) { }

void AppDispatcher::setup() {
    display.initialise();
}

void AppDispatcher::run() {
    while (true) {
        if (!selectionContext.getIsModeSelected()) {
            modeController.handleModeSelection();
        }
        else if (!selectionContext.getIsFileRemoteSelected() && selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FILES) {
            remoteController.handleFileRemoteSelection();
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
