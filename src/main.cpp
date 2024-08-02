#include <M5Cardputer.h>
#include <iostream>
#include <array>
#include <string.h>
#include "remote_api.h"
#include "display_manager.h"
#include "selection.h"
#include "ir_manager.h"
#include "keyboard.h"

DisplayManager display;                                     // Render screens
CustomKeyboard keyboard;                                    // Input handler
RemoteApi remoteApi;                                        // Remote data api to get Manufacturer, Product, Remote, RemoteCommand
IrManager irManager;                                        // IR manager to send IR sequence with the right protocol
Selection selection(display, keyboard,                      // Selection handler for screens
                    remoteApi.getEmptyManufacturer(), 
                    remoteApi.getEmptyProduct(), 
                    remoteApi.getEmptyRemote(), 
                    remoteApi.getEmptyRemoteCommand());

bool isModeSelected = false;                                // Track mode selection
bool isManufacturerSelected = false;                        // Track manufacturer selection
bool isProductSelected = false;                             // Track product selection
bool isRemoteSelected = false;                              // Track remote selection

uint16_t currentManufacturerIndex;                          // Index of the current manufacturer selection, used to go back to position
uint16_t currentProductIndex;                               // Index of the current product selection, used to go back to position
uint16_t currentRemoteIndex;                                // Index of the current remote selection, used to go back to position
uint16_t currentRemoteCommandIndex;                         // Index of the current remote command selection, used to go back to position

SelectionMode currentSelectedMode;                          // Current selected enum SelectionMode
Manufacturer currentSelectedManufacturer;                   // Current Manufacturer
Product currentSelectedProduct;                             // Current Product attached to a Manufacturer
Remote currentSelectedRemote;                               // Current Remote attached to a Product
RemoteCommand currentSelectedRemoteCommand;                 // Current RemoteCommands attached to a Remote

void setup() {

    // Initialisation M5Cardputer
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);

    // Initialisation ecran
    display.initialise();

    // Initialisation IR
    irManager.begin();
}

void loop() {

    // MODE SELECTION
    if (!isModeSelected) {
        currentSelectedMode = selection.modeSelection();
        isModeSelected = true;
    }

    // FAVORITES SELECTION
    if (currentSelectedMode == SelectionMode::FAVORITES) {
        
        // We get favorite remotes into a product to be able to use the existing logic
        // It parses each FavoriteRemote into Remote struct and attach it to a Product
        currentSelectedProduct = remoteApi.getFavoriteRemotesProduct();

        // We don't need this to use Favorites mode, set to true to skip selection
        isManufacturerSelected = true;
        isProductSelected = true;

        // At this point, we will jump to Remote selection
    }

    // MANUFACTURER SELECTION (SCAN mode, POPULARS mode, ALL_REMOTES mode)
    if (!isManufacturerSelected) {
        std::vector<std::string> manufacturers;
        std::string manufacturerName;
        
        // We only retrieve names, otherwise, we will load the entire data into RAM
        if (currentSelectedMode == SelectionMode::POPULARS) {
            manufacturers = remoteApi.getPopularManufacturerNames();
        } else {
            manufacturers = remoteApi.getAllManufacturerNames();
        }

        // If user hits the return button, manufacturerName is ""
        manufacturerName = selection.manufacturerSelection(
                manufacturers, 
                currentManufacturerIndex,  
                (currentSelectedMode == SelectionMode::SCAN)
        );

        // and it will return an empty manufacturer
        currentSelectedManufacturer = remoteApi.getManufacturerByName(manufacturerName);

        if (currentSelectedManufacturer == remoteApi.getEmptyManufacturer()) {
            isModeSelected = false;
            currentManufacturerIndex = 0;
            return; // return to start of the main loop()
        } else {
            isManufacturerSelected = true;
        }
    }

    // SCAN SELECTION
    if (currentSelectedMode == SelectionMode::SCAN) {
        std::vector<Product> products = remoteApi.getManufacturerProducts(currentSelectedManufacturer);
        std::string favoriteName;
        bool last = false;

        for (auto it = products.begin(); it != products.end(); ++it) {
            const auto& product = *it; // to be able to track the last product
            if (std::next(it) == products.end()) {
                last = true;
            }

            std::vector<Remote> remotes = remoteApi.getProductRemotes(product);

            Remote selectedRemote = selection.scanSelection(remotes, currentSelectedManufacturer.name, 
            [&](const RemoteCommand& command) { // to send a IR command
                irManager.sendRemoteCommand(command, remoteApi.getProtocolString(command.protocol));
            }, favoriteName, last);


            if (selectedRemote != remoteApi.getEmptyRemote() && favoriteName != "") {
                remoteApi.saveUserFavoriteRemote(
                    currentSelectedManufacturer.name, product.name, 
                    selectedRemote.fileName, favoriteName
                );
                break;
            }
        }
        currentManufacturerIndex = 0;
        isManufacturerSelected = false;
        isModeSelected = false;
        return;
    }


    // PRODUCT SELECTION
    if (!isProductSelected) {
        std::vector<Product> products = remoteApi.getManufacturerProducts(currentSelectedManufacturer);

        // If user hits the return button, an empty product is returned
        currentSelectedProduct = selection.productSelection(products, currentSelectedManufacturer.name, currentProductIndex);

        if (currentSelectedProduct == remoteApi.getEmptyProduct()) {
            isManufacturerSelected = false;
            currentProductIndex = 0;
            return; // return to start of the main loop()
        } else {
            isProductSelected = true;
        }
    }

    // REMOTE SELECTION
    if (!isRemoteSelected) {
        std::vector<Remote> remotes = remoteApi.getProductRemotes(currentSelectedProduct);

        // If user hits the return button, an empty remote is returned
        currentSelectedRemote = selection.remoteSelection(remotes, currentSelectedProduct.name, currentRemoteIndex);

        if (currentSelectedRemote == remoteApi.getEmptyRemote()) {
            isProductSelected = false;
            currentRemoteIndex = 0;

            if (currentSelectedMode == SelectionMode::FAVORITES) {
                isManufacturerSelected = false;
                isModeSelected = false; 
            }

            return; // return to start of the main loop()
        } else {
            isRemoteSelected = true;
        }
    }

    // REMOTE COMMANDS SELECTION
    if (isRemoteSelected) {
        std::vector<RemoteCommand> commands = remoteApi.getRemoteCommands(currentSelectedRemote);
        currentRemoteCommandIndex = 0; // Passed by reference to maintain the selection cursor position

        if (currentSelectedMode == SelectionMode::FAVORITES) {
            std::vector<FavoriteRemote> favoriteRemotes = remoteApi.getAllFavoriteRemotes();

            // Copy the product name of the remote, used to save/delete favorites
            strncpy(currentSelectedProduct.name, favoriteRemotes[currentRemoteIndex].productName, sizeof(currentSelectedProduct.name) - 1);
            currentSelectedProduct.name[sizeof(currentSelectedProduct.name) - 1] = '\0';
        }

        while (true) {

            // If user hits the return button, an empty command is returned
            RemoteCommand command = selection.remoteCommandSelection(
                commands, 
                currentSelectedRemote.fileName,
                currentSelectedProduct.name,
                currentRemoteCommandIndex,
                remoteApi.getUserFavoriteRemotes(),
                currentSelectedMode == SelectionMode::FAVORITES,
                [&](std::string filename, std::string favoriteName) { // to save a remote to favorite
                    return remoteApi.saveUserFavoriteRemote(currentSelectedManufacturer.name, currentSelectedProduct.name, filename, favoriteName);
                },
                [&](std::string favoriteName) {  // to delete a remote from favorite
                    return remoteApi.deleteUserFavoriteRemote(favoriteName, currentSelectedProduct.name);
                }
            );                                       

            if (command == remoteApi.getEmptyRemoteCommand()) {
                isRemoteSelected = false;
                break; // break the current infinite loop and return to start of the main loop()
            } else {
                // Send the IR command
                irManager.sendRemoteCommand(command, remoteApi.getProtocolString(command.protocol));
            }
        }
    }  
}
