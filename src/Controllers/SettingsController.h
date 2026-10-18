#pragma once
#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Interfaces/IAppPlatform.h>
#include <Services/InfraredService.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>

namespace controllers {
class SettingsController {
public:
    SettingsController(interfaces::IDisplayView& view, interfaces::IInputProvider& keys,
                       services::InfraredService& infrared, interfaces::IAppPlatform& platform)
        : display(view), input(keys), infraredService(infrared), platform(platform),
          globalContext(contexts::GlobalContext::getInstance()),
          selectionContext(contexts::SelectionContext::getInstance()) {}
    void handle();
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    services::InfraredService& infraredService;
    interfaces::IAppPlatform& platform;
    contexts::GlobalContext& globalContext;
    contexts::SelectionContext& selectionContext;
};
}
