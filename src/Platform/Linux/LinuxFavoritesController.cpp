#ifndef ARDUINO
#include "LinuxFavoritesController.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedZBack.h>
#include <Selections/ConfirmationSelection.h>
#include <Utils/FavoriteLabelUtils.h>
#include <Models/Remote.h>
#include <Models/Product.h>
#include <Models/Manufacturer.h>
#include <vector>
#include <string>

namespace platform::linux {
using namespace selections;
using namespace models;
void LinuxFavoritesController::handle() {
    inputs::ScopedZBack zBack(input);
    size_t selected = 0;
    while (true) {
        const auto userBuiltins = remoteService.getUserFavoriteRemotes();
        const auto online = irdbBrowser.globalFavorites();
        const auto allBuiltins = remoteService.getAllFavoriteRemotes();
        // User favorites (built-in and online) precede bundled universal remotes.
        std::vector<std::string> labels, details;
        for (const auto& fav : userBuiltins) {
            labels.push_back(fav.favoriteName[0] ? fav.favoriteName : fav.remoteFileName);
            details.push_back("Built-in");
        }
        for (const auto& item : online) {
            labels.push_back(item.label);
            details.push_back("Online IR");
        }
        for (size_t i = userBuiltins.size(); i < allBuiltins.size(); ++i) {
            const auto& fav = allBuiltins[i];
            labels.push_back(utils::universalFavoriteLabel(
                fav.favoriteName[0] ? fav.favoriteName : fav.remoteFileName));
            details.push_back("Universal");
        }
        const size_t onlineStart = userBuiltins.size();
        const size_t onlineEnd = onlineStart + online.size();
        if (selected >= labels.size()) selected = 0;
        display.displayTopBar("Favorite remotes", true);
        display.displaySelection(labels, static_cast<uint16_t>(selected), details);
        char key;
        do { key = input.handler(); } while (key == KEY_NONE);
        if (key == KEY_RETURN) break;
        if (key == KEY_ARROW_DOWN && !labels.empty()) selected = (selected + 1) % labels.size();
        else if (key == KEY_ARROW_UP && !labels.empty()) selected = selected ? selected - 1 : labels.size() - 1;
        else if (key == KEY_FAVORITE && selected < onlineEnd) {
            // Global Favorites is the actual Linux entry point for both types.
            // Nothing is deleted until OK explicitly confirms this specific item.
            ConfirmationSelection confirmation(display, input);
            if (!confirmation.select("Delete favorite: " + labels[selected], true)) continue;
            if (selected >= onlineStart) {
                if (!irdbBrowser.removeGlobalFavorite(online[selected - onlineStart].path)) {
                    display.displayMessage("Save failed", {"Favorite was not removed"});
                    while (true) {
                        const char response = input.handler();
                        if (response == KEY_OK || response == KEY_RETURN) break;
                    }
                }
            } else {
                const auto& fav = userBuiltins[selected];
                remoteService.removeFavoriteRemote(fav.favoriteName, fav.productName);
            }
        } else if (key == KEY_OK && selected < labels.size()) {
            if (selected >= onlineStart && selected < onlineEnd) {
                const auto remote = irdbBrowser.openGlobalFavorite(online[selected - onlineStart].path);
                if (remote.commands.empty()) continue;
                selectionContext.setCurrentSelectedFileRemote(remote);
                selectionContext.setCurrentFileRemoteCommandIndex(0);
                selectionContext.setIsFileRemoteSelected(true);
                commandController.handleFileRemoteCommandSelection();
            } else {
                const auto& fav = allBuiltins[selected < onlineStart ? selected : selected - online.size()];
                const Manufacturer manufacturer = manufacturerService.getManufacturer(fav.manufacturerName);
                const Product product = productService.getProduct(manufacturer, fav.productName);
                const Remote remote = remoteService.getFavoriteRemoteToRemote(manufacturer, product, fav);
                if (remote == remoteService.getEmptyRemote()) continue;
                selectionContext.setCurrentSelectedManufacturer(manufacturer);
                selectionContext.setCurrentSelectedProduct(product);
                selectionContext.setCurrentSelectedRemote(remote);
                selectionContext.setCurrentRemoteIndex(static_cast<uint16_t>(selected));
                selectionContext.setCurrentRemoteCommandIndex(0);
                selectionContext.setIsRemoteSelected(true);
                commandController.handleRemoteCommandSelection();
            }
        }
    }
    selectionContext.setIsModeSelected(false);
}
}
#endif
