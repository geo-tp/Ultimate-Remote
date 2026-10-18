#include "RemoteController.h"
#include <Utils/StringUtils.h>
#include <Utils/FavoriteLabelUtils.h>
#include <Selections/StringPromptSelection.h>

namespace controllers {

RemoteController::RemoteController(interfaces::IDisplayView& display, interfaces::IInputProvider& input, RemoteService& remoteService,
                                   ProductService& productService, ManufacturerService& manufacturerService,
                                   FileService& fileService, SdService& sdService, RemoteSelection& remoteSelection, 
                                   FilePathSelection& filepathSelection)
    : display(display), input(input), remoteService(remoteService), productService(productService),
      manufacturerService(manufacturerService), fileService(fileService), sdService(sdService),
      remoteSelection(remoteSelection), filepathSelection(filepathSelection) {}

void RemoteController::handleFileRemoteSelection() {
    FilePathSelection filePathSelection(display, input);
    ConfirmationSelection confirmationSelection(display, input);
    std::string fileContent;
    std::string fileName;
    std::string fileExt;
    std::vector<std::string> elementNames;

    selectionContext.setCurrentFileRemoteIndex(0);
    cachedDirectoryElements.clear(); // Fresh listings for each SD mount/session.
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
    uint16_t rememberedIndex = selectionContext.getCurrentRemoteIndex();
    for (;;) {
        bool refreshAfterFavorite = false;
        std::vector<Remote> remotes;
        std::vector<Product> remoteProducts;
        std::vector<std::string> remoteLabels;
        bool isManufacturerMode = selectionContext.getCurrentSelectedMode() == SelectionModeEnum::ALL_REMOTES;
        std::string title;

        if (isManufacturerMode) {
            Manufacturer manufacturer = selectionContext.getCurrentSelectedManufacturer();
            title = manufacturer.name;
            for (const auto& product : productService.getManufacturerProducts(manufacturer)) {
                for (const auto& remote : remoteService.getRemotes(product)) {
                    remotes.push_back(remote);
                    remoteProducts.push_back(product);
                    remoteLabels.push_back(std::string(remote.fileName) + " - " + product.name);
                }
            }
        } else if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES) {
            std::vector<FavoriteRemote> favoriteRemotes = remoteService.getAllFavoriteRemotes();
            const size_t personalCount = remoteService.getUserFavoriteRemotes().size();
            size_t favoriteIndex = 0;
            for (const auto& favorite : favoriteRemotes) {
                Manufacturer manufacturer = manufacturerService.getManufacturer(favorite.manufacturerName);
                Product product = productService.getProduct(manufacturer, favorite.productName);
                Remote remote = remoteService.getFavoriteRemoteToRemote(manufacturer, product, favorite);
                if (remote != remoteService.getEmptyRemote()) {
                    remotes.push_back(remote);
                    remoteProducts.push_back(product);
                    // User-defined names are left untouched; only factory Universal
                    // preset names are expanded for display (IDs remain unchanged).
                    const std::string label = favoriteIndex >= personalCount ?
                        utils::universalFavoriteLabel(remote.fileName) : std::string(remote.fileName);
                    remoteLabels.push_back(label);
                }
                ++favoriteIndex;
            }
            selectionContext.setCurrentSelectedProduct(productService.getFavoriteRemotesProduct(remotes));
        } else {
            remotes = remoteService.getRemotes(selectionContext.getCurrentSelectedProduct());
        }

        if (!isManufacturerMode) {
            title = selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES
                ? "Favorite remotes" : selectionContext.getCurrentSelectedProduct().name;
        }
        uint16_t currentRemoteIndex = rememberedIndex;
        if (remotes.empty()) currentRemoteIndex = 0;
        Remote selectedRemote = remoteSelection.select(remotes, title, currentRemoteIndex,
                                                       remoteService.getEmptyRemote(), remoteLabels,
            (selectionContext.getCurrentSelectedMode() != SelectionModeEnum::FAVORITES) ? std::function<void(size_t)>([&](size_t index) {
                if (index >= remotes.size()) return;
                const Product product = isManufacturerMode ? remoteProducts[index] : selectionContext.getCurrentSelectedProduct();
                const auto& remote = remotes[index];
                const auto favorites = remoteService.getUserFavoriteRemotes();
                for (const auto& fav : favorites) {
                    if (std::string(fav.manufacturerName) == selectionContext.getCurrentSelectedManufacturer().name &&
                        std::string(fav.productName) == product.name &&
                        std::string(fav.remoteFileName) == remote.fileName) {
                        ConfirmationSelection confirm(display, input);
                        if (confirm.select("Delete favorite: " + std::string(fav.favoriteName), true))
                            remoteService.removeFavoriteRemote(fav.favoriteName, fav.productName);
                        return;
                    }
                }
                StringPromptSelection prompt(display, input);
                const std::string favoriteName = prompt.select("Type a favorite name");
                if (!favoriteName.empty())
                    remoteService.addFavoriteRemote(selectionContext.getCurrentSelectedManufacturer().name,
                                                    product.name, remote.fileName, favoriteName);
            }) : std::function<void(size_t)>([&](size_t index) {
                if (index >= remotes.size()) return;
                const auto favorites = remoteService.getAllFavoriteRemotes();
                // Remote list is built in the same order, skipping invalid entries.
                size_t valid = 0;
                for (const auto& fav : favorites) {
                    const Manufacturer manufacturer = manufacturerService.getManufacturer(fav.manufacturerName);
                    const Product product = productService.getProduct(manufacturer, fav.productName);
                    const Remote remote = remoteService.getFavoriteRemoteToRemote(manufacturer, product, fav);
                    if (remote == remoteService.getEmptyRemote()) continue;
                    if (valid++ != index) continue;
                    const auto personal = remoteService.getUserFavoriteRemotes();
                    for (const auto& item : personal) {
                        if (std::string(item.favoriteName) == std::string(fav.favoriteName) &&
                            std::string(item.productName) == std::string(fav.productName)) {
                            ConfirmationSelection confirm(display, input);
                            if (confirm.select("Delete favorite: " + std::string(item.favoriteName), true)) {
                                const size_t before = personal.size();
                                const auto remaining = remoteService.removeFavoriteRemote(item.favoriteName, item.productName);
                                refreshAfterFavorite = input.refreshFavoritesAfterDelete() && remaining.size() < before;
                            }
                            return;
                        }
                    }
                    return; // Universal built-ins cannot be removed.
                }
            }), &refreshAfterFavorite);

        if (refreshAfterFavorite) {
            rememberedIndex = currentRemoteIndex;
            continue; // Reload from persisted favorites; never show the deleted row.
        }

        if (selectedRemote == remoteService.getEmptyRemote()) {
            selectionContext.setIsRemoteSelected(false);
            selectionContext.setIsProductSelected(false);
            selectionContext.setCurrentRemoteIndex(0);
            selectionContext.setCurrentRemoteCommandIndex(0);

            if (isManufacturerMode) {
                selectionContext.setIsManufacturerSelected(false);
            }

            if (selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES) {
                selectionContext.setIsManufacturerSelected(false);
                selectionContext.setIsModeSelected(false);
            }
        } else {
            if ((isManufacturerMode || selectionContext.getCurrentSelectedMode() == SelectionModeEnum::FAVORITES)
                && currentRemoteIndex < remoteProducts.size()) {
                selectionContext.setCurrentSelectedProduct(remoteProducts[currentRemoteIndex]);
                selectionContext.setIsProductSelected(true);
            }
            selectionContext.setIsRemoteSelected(true);
            selectionContext.setCurrentSelectedRemote(selectedRemote);
            selectionContext.setCurrentRemoteIndex(currentRemoteIndex);
            selectionContext.setCurrentRemoteCommandIndex(0);
        }
        break;
    } // Rebuild after deleting a favorite on ESP32.
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
            if (cachedDirectoryElements.size() >= static_cast<size_t>(std::max(0, globalContext.getFileCacheLimit()))) {
                cachedDirectoryElements.erase(cachedDirectoryElements.begin());
            }

            cachedDirectoryElements[path] = elements;
        }

        return elements;
    }
}

} // namespace controllers
