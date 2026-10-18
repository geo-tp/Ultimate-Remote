#pragma once
#include <Interfaces/IAppPlatform.h>
#include "ArduinoFavoritesStorage.h"

namespace controllers { class RemoteController; }

namespace platform::arduino {
class ArduinoAppPlatform final : public interfaces::IAppPlatform {
public:
    interfaces::IFavoritesStorage& favoriteStorage() override { return storage; }
    void attach(const platform::AppPlatformDependencies& dependencies) override;
    void openFiles() override;
    void openFavorites() override;
    bool supportsMode(enums::SelectionModeEnum mode) const override { return mode != enums::SelectionModeEnum::INFO; }
    const char* fileMenuName() const override { return "IR FILES"; }
    const char* fileMenuDescription() const override { return "Flipper .ir files on SD"; }
    const char* irPinHint() const override { return "Change output pin"; }
    bool saveScreenTimeout() override { return true; }
    void applyIrPin(int pin, services::InfraredService& infrared,
                    interfaces::IDisplayView& display) override;
private:
    ArduinoFavoritesStorage storage;
    controllers::RemoteController* remoteController = nullptr;
};
}
