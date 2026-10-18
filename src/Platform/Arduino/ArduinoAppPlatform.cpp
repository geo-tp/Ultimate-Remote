#ifdef ARDUINO
#include "ArduinoAppPlatform.h"
#include <Platform/AppPlatformDependencies.h>
#include <Controllers/RemoteController.h>
#include <Services/InfraredService.h>
#include <Interfaces/IDisplayView.h>
#include <string>

namespace platform::arduino {
void ArduinoAppPlatform::attach(const platform::AppPlatformDependencies& dependencies) {
    remoteController = &dependencies.remotes;
}
void ArduinoAppPlatform::openFiles() {
    if (remoteController) remoteController->handleFileRemoteSelection();
}
void ArduinoAppPlatform::openFavorites() {
    if (remoteController) remoteController->handleRemoteSelection();
}
void ArduinoAppPlatform::applyIrPin(int pin, services::InfraredService& infrared,
                                   interfaces::IDisplayView& display) {
    infrared.begin();
    display.displayMessage("IR TX GPIO saved", {"GPIO " + std::to_string(pin)});
}
}
#endif
