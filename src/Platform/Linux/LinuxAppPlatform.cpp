#ifndef ARDUINO
#include "LinuxAppPlatform.h"
#include <Platform/AppPlatformDependencies.h>
#include "LinuxIrdbBrowser.h"
#include <Platform/Linux/LinuxFavoritesController.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <Interfaces/IDisplayView.h>
#include <string>

namespace platform::linux {
LinuxAppPlatform::LinuxAppPlatform() = default;
LinuxAppPlatform::~LinuxAppPlatform() = default;

void LinuxAppPlatform::attach(const platform::AppPlatformDependencies& dependencies) {
    browser = std::make_unique<LinuxIrdbBrowser>(dependencies.display, dependencies.input, dependencies.files);
    favorites = std::make_unique<platform::linux::LinuxFavoritesController>(
        dependencies.display, dependencies.input, dependencies.remoteService,
        dependencies.products, dependencies.manufacturers, dependencies.commands, *browser);
}
void LinuxAppPlatform::openFiles() {
    if (!browser) return;
    auto& selection = contexts::SelectionContext::getInstance();
    const auto remote = browser->select();
    if (remote.commands.empty()) {
        selection.setIsModeSelected(false);
    } else {
        selection.setCurrentSelectedFileRemote(remote);
        selection.setCurrentFileRemoteCommandIndex(0);
        selection.setIsFileRemoteSelected(true);
    }
}
void LinuxAppPlatform::openFavorites() {
    if (favorites) favorites->handle();
}
bool LinuxAppPlatform::saveScreenTimeout() {
    return contexts::GlobalContext::getInstance().saveSettings();
}
void LinuxAppPlatform::applyIrPin(int pin, services::InfraredService&,
                                  interfaces::IDisplayView& display) {
    const bool saved = contexts::GlobalContext::getInstance().saveSettings();
    display.displayMessage(saved ? "IR GPIO preference saved" : "Could not save settings", {
        "Set dtoverlay=gpio-ir-tx,",
        "gpio_pin=" + std::to_string(pin) + " then reboot"
    });
}
}
#endif
