#pragma once
#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Services/InfraredService.h>
#include <Services/LedService.h>
#include <Contexts/SelectionContext.h>

namespace controllers {
class IrBuilderController {
public:
    IrBuilderController(interfaces::IDisplayView& view, interfaces::IInputProvider& keys,
                        services::InfraredService& infrared, services::LedService& led)
        : display(view), input(keys), infraredService(infrared), ledService(led),
          selectionContext(contexts::SelectionContext::getInstance()) {}
    void handle();
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    services::InfraredService& infraredService;
    services::LedService& ledService;
    contexts::SelectionContext& selectionContext;
};
}
