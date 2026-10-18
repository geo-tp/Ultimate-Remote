#pragma once
#include <Interfaces/IDisplayView.h>
#include <Interfaces/IInputProvider.h>
#include <Repositories/ManufacturerRepository.h>
#include <Contexts/SelectionContext.h>

namespace controllers {
// Info screen navigation; dispatcher only selects the active controller.
class InfoController {
public:
    InfoController(interfaces::IDisplayView& view, interfaces::IInputProvider& keys,
                   repositories::ManufacturerRepository& manufacturers)
        : display(view), input(keys), manufacturerRepository(manufacturers),
          selectionContext(contexts::SelectionContext::getInstance()) {}
    void handle();
private:
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    repositories::ManufacturerRepository& manufacturerRepository;
    contexts::SelectionContext& selectionContext;
};
}
