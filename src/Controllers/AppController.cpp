#include "AppController.h"


namespace controllers {

AppController::AppController(CardputerView& display, CardputerInput& input)
    : display(display), input(input),
      isModeSelected(false), isManufacturerSelected(false),
      isProductSelected(false), isRemoteSelected(false),
      currentManufacturerIndex(0), currentProductIndex(0),
      currentRemoteIndex(0), currentRemoteCommandIndex(0) {}

void AppController::setup() {
    display.initialise();
    infraredService.begin();
}

void AppController::run() {
    while (true) {
        if (!isModeSelected) {
            handleModeSelection();
        }
        else if(!isFileRemoteSelected && currentSelectedMode == SelectionMode::FILES) {
            handleFileRemoteSelection();
        }
        else if (isFileRemoteSelected && currentSelectedMode == SelectionMode::FILES) {
            handleFileRemoteCommandSelection();
        }
        else if (!isManufacturerSelected && currentSelectedMode != SelectionMode::FAVORITES) {
            handleManufacturerSelection();
        }
        else if (currentSelectedMode == SelectionMode::SCAN) {
            handleScanSelection();
        }
        else if (!isProductSelected && currentSelectedMode != SelectionMode::SCAN) {
            handleProductSelection();
        }
        else if (!isRemoteSelected) {
            handleRemoteSelection();
        }
        else if (isRemoteSelected) {
            handleRemoteCommandSelection();
        }
    }
}

void AppController::handleModeSelection() {
    ModeSelection modeSelection(display, input);
    currentSelectedMode = modeSelection.select();
    isModeSelected = true;
 
    if (currentSelectedMode == SelectionMode::FAVORITES) {
        isManufacturerSelected = true;
        isProductSelected = true;
    }
}

void AppController::handleManufacturerSelection() {
    ManufacturerSelection manufacturerSelection(display, input);
    std::vector<std::string> manufacturers;
    manufacturers = manufacturerService.getAllManufacturerNames();

    std::string manufacturerName = manufacturerSelection.select(manufacturers, currentManufacturerIndex, (currentSelectedMode == SelectionMode::SCAN));
    currentSelectedManufacturer = manufacturerService.getManufacturerByName(manufacturerName);

    if (currentSelectedManufacturer == manufacturerService.getEmptyManufacturer()) {
        isModeSelected = false;
        currentManufacturerIndex = 0;
    } else {
        isManufacturerSelected = true;
    }
}

void AppController::handleScanSelection() {
    ScanSelection scanSelection = ScanSelection(display, input);
    std::vector<Product> products = productService.getManufacturerProducts(currentSelectedManufacturer);
    std::string favoriteName;
    bool last = false;

    for (auto it = products.begin(); it != products.end(); ++it) {
        // To track the last product
        const auto& product = *it;
        if (std::next(it) == products.end()) {
            last = true;
        }

        std::vector<Remote> remotes = remoteService.getRemotes(product);

        Remote selectedRemote = scanSelection.select(remotes, currentSelectedManufacturer.name, 
        [&](const RemoteCommand& command, std::string manufacturerName) { // Send Remote Command function passed as param
            infraredService.sendRemoteCommand(command, manufacturerName);
        }, favoriteName, last, remoteService.getEmptyRemote());

        if (selectedRemote != remoteService.getEmptyRemote() && !favoriteName.empty()) {
            remoteService.addFavoriteRemote(currentSelectedManufacturer.name, product.name, selectedRemote.fileName, favoriteName);
            break;
        }
    }
    currentManufacturerIndex = 0;
    isManufacturerSelected = false;
    isModeSelected = false;
}

void AppController::handleProductSelection() {
    ProductSelection productSelection(display, input);
    std::vector<Product> products = productService.getManufacturerProducts(currentSelectedManufacturer);
    Product emptyProduct = productService.getEmptyProduct();
    currentSelectedProduct = productSelection.select(products, currentSelectedManufacturer.name, currentProductIndex, emptyProduct);

    if (currentSelectedProduct == productService.getEmptyProduct()) {
        isManufacturerSelected = false;
        currentProductIndex = 0;
    } else {
        isProductSelected = true;
    }
}

void AppController::handleRemoteSelection() {
    RemoteSelection remoteSelection(display, input);
    std::vector<Remote> remotes;

    if (currentSelectedMode == SelectionMode::FAVORITES) {
        std::vector<FavoriteRemote> favoriteRemotes = remoteService.getAllFavoriteRemotes();
        for (const auto& favorite : favoriteRemotes) {
            Manufacturer manufacturer = manufacturerService.getManufacturerByName(favorite.manufacturerName);
            Product product = productService.getProduct(manufacturer, favorite.productName);
            Remote remote = remoteService.getFavoriteRemoteToRemote(manufacturer, product, favorite);
            if (remote != remoteService.getEmptyRemote()) {
                remotes.push_back(remote);
            }
        }
        currentSelectedProduct = productService.getFavoriteRemotesProduct(remotes);
    } else {
        remotes = remoteService.getRemotes(currentSelectedProduct);
    }

    currentSelectedRemote = remoteSelection.select(remotes, currentSelectedProduct.name, currentRemoteIndex, remoteService.getEmptyRemote());

    if (currentSelectedRemote == remoteService.getEmptyRemote()) {
        isProductSelected = false;
        currentRemoteIndex = 0;

        if (currentSelectedMode == SelectionMode::FAVORITES) {
            isManufacturerSelected = false;
            isModeSelected = false;
        }
    } else {
        isRemoteSelected = true;
    }
}

void AppController::handleRemoteCommandSelection() {
    RemoteCommandSelection remoteCommandSelection(display, input);
    std::vector<RemoteCommand> commands = remoteService.getRemoteCommands(currentSelectedRemote);
    currentRemoteCommandIndex = 0;
    bool isFavoriteMode = currentSelectedMode == SelectionMode::FAVORITES;

    if (isFavoriteMode) {
        std::vector<FavoriteRemote> favoriteRemotes = remoteService.getAllFavoriteRemotes();
        currentSelectedProduct = productService.setProductName(currentSelectedProduct, favoriteRemotes[currentRemoteIndex].productName);
    }

    while (true) {
        RemoteCommand command = remoteCommandSelection.select(
            commands, 
            currentSelectedRemote.fileName,
            currentSelectedProduct.name,
            currentRemoteCommandIndex,
            remoteService.getUserFavoriteRemotes(),
            isFavoriteMode,
            [&](std::string filename, std::string favoriteName) { // Add Favorite remote function as param
                return remoteService.addFavoriteRemote(currentSelectedManufacturer.name, currentSelectedProduct.name, filename, favoriteName);
            },
            [&](std::string favoriteName) {  // Delete Favorite remote function as param
                return remoteService.removeFavoriteRemote(favoriteName, currentSelectedProduct.name);
            },
            remoteService.getEmptyRemoteCommand()
        );

        if (command == remoteService.getEmptyRemoteCommand()) {
            isRemoteSelected = false;
            break;
        } else {
            infraredService.sendRemoteCommand(command, currentSelectedManufacturer.name);
            ledService.blink();
        }
    }
}

void AppController::handleFileRemoteSelection() {
    FilePathSelection filePathSelection(display, input);
    ConfirmationSelection confirmationSelection(display, input);
    std::string fileContent;
    std::string fileName;
    std::string fileExt;
    std::string parentDir;
    std::vector<std::string> elementNames;
    currentFileRemoteIndex = 0;

    sdService.begin();

    if (!sdService.getSdState()) {
        confirmationSelection.select("SD card not found");
        isModeSelected = false;
        sdService.close();
        return;
    }

    do {
        currentFileRemoteIndex = 0;
        display.displayLoading();
        elementNames = getCachedDirectoryElements(currentSelectedFilePath, sdService);

        if (sdService.isFile(currentSelectedFilePath)) {
            fileExt = StringUtils::extractFileExtension(currentSelectedFilePath);

            if (fileExt != "ir")  {
                // NOT A VALID EXT FILE
                fileContent = ""; // better to not load anything, it could make the app crashes
            } else {
                fileContent = sdService.readFile(currentSelectedFilePath.c_str());
            }

            if (fileService.validateInfraredFile(fileContent)) {
                // VALID IR CONTENT
                fileName = StringUtils::extractFilename(currentSelectedFilePath);
                currentSelectedFileRemote = fileService.getRemoteFromFile(fileName, fileContent);
                isFileRemoteSelected = true;
                break;
            } else {
                // NOT A VALID IR CONTENT
                confirmationSelection.select("Not a valid .ir file");
                currentSelectedFilePath = StringUtils::getParentDirectory(currentSelectedFilePath);
                elementNames = getCachedDirectoryElements(currentSelectedFilePath, sdService);
            }
        }

        // At this point filepath can only be a folder
        currentSelectedFilePath = filePathSelection.select(elementNames, currentSelectedFilePath, currentFileRemoteIndex, fileSelectionFirstRun);
        fileSelectionFirstRun = false; // display info screen only one time

    } while (!currentSelectedFilePath.empty()); // empty filepath means user hits return button at root level "/"

    if (currentSelectedFilePath.empty()) {
        // Reset to go back to menu
        isModeSelected = false;
        currentSelectedFilePath = "/";
    } else {
        // To be able to return to the remote folder
        currentSelectedFilePath = StringUtils::getParentDirectory(currentSelectedFilePath);
    }

    sdService.close();
}

void AppController::handleFileRemoteCommandSelection() {
    FileRemoteCommandSelection fileRemoteCommandSelection(display, input);
    currentFileRemoteCommandIndex = 0;

    while (true) {
        FileRemoteCommand command = fileRemoteCommandSelection.select(
            currentSelectedFileRemote.commands, 
            currentSelectedFileRemote.fileName,
            currentFileRemoteCommandIndex,
            remoteService.getEmptyFileRemoteCommand()
        );

        if (command == remoteService.getEmptyFileRemoteCommand()) {
            isFileRemoteSelected = false;
            break;
        } else {
            infraredService.sendFileRemoteCommand(command, currentSelectedFileRemote.fileName);
            ledService.blink();
        }
    }
}

std::vector<std::string> AppController::getCachedDirectoryElements(const std::string& path, SdService& dataService) {
    // Check if the directory is already in the cache
    if (cachedDirectoryElements.find(path) != cachedDirectoryElements.end()) {
        return cachedDirectoryElements[path];
    } else {
        // Get elements from the service
        std::vector<std::string> elements = dataService.listElements(path);

        // Store in the cache only if there are more than 4 elements
        if (elements.size() > 4) {
            // Remove the first element if the cache limit is reached
            if (cachedDirectoryElements.size() >= context.getFileCacheLimit()) {
                cachedDirectoryElements.erase(cachedDirectoryElements.begin());
            }

            cachedDirectoryElements[path] = elements;
        }

        return elements;
    }
}


}