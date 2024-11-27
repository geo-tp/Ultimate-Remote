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

using namespace views;
using namespace inputs;
using namespace services;
using namespace repositories;
using namespace selections;
using namespace contexts;
using namespace enums;
using namespace models;

namespace controllers {

class RemoteController {
public:
    RemoteController(CardputerView& display, CardputerInput& input, RemoteService& remoteService,
                     ProductService& productService, ManufacturerRepository& manufacturerService,
                     FileService& fileService, SdService& sdService, RemoteSelection& remoteSelection, 
                     FilePathSelection& filepathSelection);

    void handleFileRemoteSelection();
    void handleRemoteSelection();

private:
    CardputerView& display;
    CardputerInput& input;
    RemoteService& remoteService;
    ProductService& productService;
    ManufacturerRepository& manufacturerService;
    FileService& fileService;
    SdService& sdService;

    RemoteSelection& remoteSelection; 
    FilePathSelection& filepathSelection;

    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();

    std::unordered_map<std::string, std::vector<std::string>> cachedDirectoryElements;
    std::vector<std::string> getCachedDirectoryElements(const std::string& path, SdService& dataService);
};

} // namespace controllers

#endif // REMOTE_CONTROLLER_H
