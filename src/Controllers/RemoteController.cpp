#include "RemoteController.h"
#include <Utils/StringUtils.h>

namespace controllers {

RemoteController::RemoteController(CardputerView& display, CardputerInput& input, RemoteService& remoteService,
                                   ProductService& productService, ManufacturerRepository& manufacturerService,
                                   FileService& fileService, SdService& sdService, GlobalContext& globalContext,
                                   SelectionContext& selectionContext, RemoteSelection& remoteSelection, FilePathSelection& filepathSelection)
    : display(display), input(input), remoteService(remoteService), productService(productService),
      manufacturerService(manufacturerService), fileService(fileService), sdService(sdService),
      globalContext(globalContext), selectionContext(selectionContext), remoteSelection(remoteSelection), filepathSelection(filepathSelection) {}

void RemoteController::handleFileRemoteSelection() {
    FilePathSelection filePathSelection(display, input);
    ConfirmationSelection confirmationSelection(display, input);
    std::string fileContent;
    std::string fileName;
    std::string fileExt;
    std::vector<std::string> elementNames;

    selectionContext.setCurrentFileRemoteIndex(0);
    sdService.begin();

    if (!sdService.getSdState()) {
        confirmationSelection.select("SD card not found");
        selectionContext.setIsModeSelected(false);
        sdService.close();
        return;
    }

    do {
        selectionContext.setCurrentFileRemoteIndex(0);
        display.displayLoading();
        elementNames = getCachedDirectoryElements(selectionContext.getCurrentSelectedFilePath(), sdService);

        if (sdService.isFile(selectionContext.getCurrentSelectedFilePath())) {
            fileExt = StringUtils::extractFileExtension(selectionContext.getCurrentSelectedFilePath());

            if (fileExt != "ir") {
                fileContent.clear();
            } else {
                fileContent = sdService.readFile(selectionContext.getCurrentSelectedFilePath().c_str());
            }

            if (fileService.validateInfraredFile(fileContent)) {
                fileName = StringUtils::extractFilename(selectionContext.getCurrentSelectedFilePath());
                selectionContext.setCurrentSelectedFileRemote(fileService.getRemoteFromFile(fileName, fileContent));
                selectionContext.setIsFileRemoteSelected(true);
                break;
            } else {
                confirmationSelection.select("Not a valid .ir file");
                selectionContext.setCurrentSelectedFilePath(StringUtils::getParentDirectory(selectionContext.getCurrentSelectedFilePath()));
                elementNames = getCachedDirectoryElements(selectionContext.getCurrentSelectedFilePath(), sdService);
            }
        }

        uint16_t currentFileRemoteIndex = selectionContext.getCurrentFileRemoteIndex();
        bool isFileSelectionFirstRun = selectionContext.getFileSelectionFirstRun();
        selectionContext.setCurrentSelectedFilePath(
            filePathSelection.select(elementNames, selectionContext.getCurrentSelectedFilePath(),
                                    currentFileRemoteIndex , isFileSelectionFirstRun));
        selectionContext.setFileSelectionFirstRun(false);

    } while (!selectionContext.getCurrentSelectedFilePath().empty());

    if (selectionContext.getCurrentSelectedFilePath().empty()) {
        selectionContext.setIsModeSelected(false);
        selectionContext.setCurrentSelectedFilePath("/");
    } else {
        selectionContext.setCurrentSelectedFilePath(StringUtils::getParentDirectory(selectionContext.getCurrentSelectedFilePath()));
    }

    sdService.close();
}

void RemoteController::handleRemoteSelection() {
    std::vector<Remote> remotes;

    if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES) {
        std::vector<FavoriteRemote> favoriteRemotes = remoteService.getAllFavoriteRemotes();
        for (const auto& favorite : favoriteRemotes) {
            Manufacturer manufacturer = manufacturerService.getManufacturerByName(favorite.manufacturerName);
            Product product = productService.getProduct(manufacturer, favorite.productName);
            Remote remote = remoteService.getFavoriteRemoteToRemote(manufacturer, product, favorite);
            if (remote != remoteService.getEmptyRemote()) {
                remotes.push_back(remote);
            }
        }
        selectionContext.setCurrentSelectedProduct(productService.getFavoriteRemotesProduct(remotes));
    } else {
        remotes = remoteService.getRemotes(selectionContext.getCurrentSelectedProduct());
    }

    uint16_t currentRemoteIndex = selectionContext.getCurrentRemoteIndex();
    Remote selectedRemote = remoteSelection.select(remotes, selectionContext.getCurrentSelectedProduct().name, 
                                                   currentRemoteIndex, remoteService.getEmptyRemote());

    if (selectedRemote == remoteService.getEmptyRemote()) {
        selectionContext.setIsProductSelected(false);
        selectionContext.setCurrentRemoteIndex(0);

        if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES) {
            selectionContext.setIsManufacturerSelected(false);
            selectionContext.setIsModeSelected(false);
        }
    } else {
        selectionContext.setIsRemoteSelected(true);
        selectionContext.setCurrentSelectedRemote(selectedRemote);
        selectionContext.setCurrentRemoteIndex(currentRemoteIndex);
    }
}

std::vector<std::string> RemoteController::getCachedDirectoryElements(const std::string& path, SdService& dataService) {
    // Check if the directory is already in the cache
    if (cachedDirectoryElements.find(path) != cachedDirectoryElements.end()) {
        return cachedDirectoryElements[path];
    } else {
        // Get elements from the service
        std::vector<std::string> elements = dataService.listElements(path);

        // Store in the cache only if there are more than 4 elements
        if (elements.size() > 4) {
            // Remove the first element if the cache limit is reached
            if (cachedDirectoryElements.size() >= globalContext.getFileCacheLimit()) {
                cachedDirectoryElements.erase(cachedDirectoryElements.begin());
            }

            cachedDirectoryElements[path] = elements;
        }

        return elements;
    }
}

} // namespace controllers
