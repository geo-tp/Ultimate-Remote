#pragma once

// Application-level platform boundary. No Linux/Arduino types leak into the
// dispatcher, controllers, or selections. Each platform owns its navigation
// and persistence implementations; these are wired once by AppDispatcher.
#include <Enums/SelectionModeEnum.h>

namespace interfaces {
class IDisplayView;
class IInputProvider;
class IFavoritesStorage;
}
namespace services { class InfraredService; }
namespace platform { struct AppPlatformDependencies; }

namespace interfaces {
class IAppPlatform {
public:
    virtual ~IAppPlatform() = default;
    virtual IFavoritesStorage& favoriteStorage() = 0;
    virtual void attach(const platform::AppPlatformDependencies& dependencies) = 0;
    virtual void openFiles() = 0;
    virtual void openFavorites() = 0;
    virtual bool supportsMode(enums::SelectionModeEnum mode) const { (void)mode; return true; }
    virtual const char* fileMenuName() const = 0;
    virtual const char* fileMenuDescription() const = 0;
    virtual const char* irPinHint() const = 0;
    virtual bool saveScreenTimeout() = 0;
    virtual void applyIrPin(int pin, services::InfraredService& infrared,
                            IDisplayView& display) = 0;
};
}
