#include "AppController.h"


namespace controllers {

AppController::AppController(CardputerView& display, CardputerInput& input)
    : display(display), input(input),
      isModeSelected(false), isManufacturerSelected(false),
      isProductSelected(false), isRemoteSelected(false),
      currentManufacturerIndex(0), currentProductIndex(0),
      currentRemoteIndex(0), currentRemoteCommandIndex(0) {}

void AppController::setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    display.initialise();
    infraredService.begin();
    ledService.begin();
}

void AppController::run() {
    while (true) {
        if (!isModeSelected) {
            handleModeSelection();
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
    if (currentSelectedMode == SelectionMode::POPULARS) {
        manufacturers = manufacturerService.getPopularManufacturerNames();
    } else {
        manufacturers = manufacturerService.getAllManufacturerNames();
    }

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
        const auto& product = *it;
        if (std::next(it) == products.end()) {
            last = true;
        }

        std::vector<Remote> remotes = remoteService.getRemotes(product);

        Remote selectedRemote = scanSelection.select(remotes, currentSelectedManufacturer.name, 
        [&](const RemoteCommand& command) { 
            const char* protocolString = protocolService.getProtocolString(command.protocol);
            infraredService.sendRemoteCommand(command, protocolString);
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
            [&](std::string filename, std::string favoriteName) {
                return remoteService.addFavoriteRemote(currentSelectedManufacturer.name, currentSelectedProduct.name, filename, favoriteName);
            },
            [&](std::string favoriteName) {
                return remoteService.removeFavoriteRemote(favoriteName, currentSelectedProduct.name);
            },
            remoteService.getEmptyRemoteCommand()
        );

        if (command == remoteService.getEmptyRemoteCommand()) {
            isRemoteSelected = false;
            break;
        } else {
            const char* protocolString = protocolService.getProtocolString(command.protocol);
            infraredService.sendRemoteCommand(command, protocolString);
            ledService.blink();
        }
    }
}

}