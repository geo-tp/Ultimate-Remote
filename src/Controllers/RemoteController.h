#ifndef REMOTE_CONTROLLER_H
#define REMOTE_CONTROLLER_H

#include <unordered_map>

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/RemoteService.h>
#include <Services/ProductService.h>
#include <Services/ManufacturerService.h>
#include <Services/FileService.h>
#include <Services/SdService.h>
#include <Selections/RemoteSelection.h>
#include <Selections/FilePathSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Enums/SelectionModeEnum.h>
#include <Models/Remote.h>
#include <Models/FileRemote.h>

namespace controllers {

class RemoteController {
public:
    RemoteController(views::CardputerView& display, inputs::CardputerInput& input, services::RemoteService& remoteService,
                     services::ProductService& productService, repositories::ManufacturerRepository& manufacturerService,
                     services::FileService& fileService, services::SdService& sdService,
                     selections::RemoteSelection& remoteSelection,
                     selections::FilePathSelection& filepathSelection);

    void handleFileRemoteSelection();
    void handleRemoteSelection();

private:
    views::CardputerView& display;
    inputs::CardputerInput& input;
    services::RemoteService& remoteService;
    services::ProductService& productService;
    repositories::ManufacturerRepository& manufacturerService;
    services::FileService& fileService;
    services::SdService& sdService;

    selections::RemoteSelection& remoteSelection;
    selections::FilePathSelection& filepathSelection;

    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    contexts::SelectionContext& selectionContext = contexts::SelectionContext::getInstance();

    std::unordered_map<std::string, std::vector<std::string>> cachedDirectoryElements;
    std::vector<std::string> getCachedDirectoryElements(const std::string& path, services::SdService& dataService);
};

} // namespace controllers

#endif // REMOTE_CONTROLLER_H
