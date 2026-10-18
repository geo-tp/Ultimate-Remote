#pragma once
#include <Interfaces/IAppPlatform.h>
#include "LinuxFavoritesStorage.h"
#include <memory>

namespace platform::linux {
class LinuxIrdbBrowser;
class LinuxFavoritesController;

class LinuxAppPlatform final : public interfaces::IAppPlatform {
public:
    LinuxAppPlatform();
    ~LinuxAppPlatform() override;
    interfaces::IFavoritesStorage& favoriteStorage() override { return storage; }
    void attach(const platform::AppPlatformDependencies& dependencies) override;
    void openFiles() override;
    void openFavorites() override;
    const char* fileMenuName() const override { return "ONLINE IR"; }
    const char* fileMenuDescription() const override { return "Flipper Database"; }
    const char* irPinHint() const override { return "Requires kernel overlay"; }
    bool saveScreenTimeout() override;
    void applyIrPin(int pin, services::InfraredService& infrared,
                    interfaces::IDisplayView& display) override;
private:
    LinuxFavoritesStorage storage;
    std::unique_ptr<LinuxIrdbBrowser> browser;
    std::unique_ptr<LinuxFavoritesController> favorites;
};
}
