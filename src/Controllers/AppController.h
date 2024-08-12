#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <sstream>

#include <Inputs/CardputerInput.h>
#include <Views/CardputerView.h>

#include <Models/Manufacturer.h>
#include <Models/Product.h>
#include <Models/Remote.h>
#include <Models/RemoteCommand.h>
#include <Models/FavoriteRemote.h>

#include <Services/InfraredService.h>
#include <Services/ProductService.h>
#include <Services/RemoteService.h>
#include <Services/LedService.h>
#include <Services/FileService.h>
#include <Services/SdService.h>
#include <Repositories/ManufacturerRepository.h>

#include <Selections/ManufacturerSelection.h>
#include <Selections/ModeSelection.h>
#include <Selections/ProductSelection.h>
#include <Selections/RemoteCommandSelection.h>
#include <Selections/FilePathSelection.h>
#include <Selections/RemoteSelection.h>
#include <Selections/ScanSelection.h>
#include <Selections/FileRemoteCommandSelection.h>


using namespace views;
using namespace inputs;
using namespace services;
using namespace models;
using namespace selections;

namespace controllers {

    class AppController {
    public:
        AppController(CardputerView& display, CardputerInput& input);
        void setup();
        void run();

    private:
        CardputerView& display;
        CardputerInput& input;

        InfraredService infraredService;
        LedService ledService;
        ProductService productService;
        RemoteService remoteService;
        FileService fileService;
        SdService sdService;

        // TODO: Find why database is imported 2x when using service
        // ManufacturerService manufacturerService;
        ManufacturerRepository manufacturerService; // using Repo for now

        bool isModeSelected;
        bool isManufacturerSelected;
        bool isProductSelected;
        bool isRemoteSelected;
        bool isFileRemoteSelected;

        uint16_t currentManufacturerIndex;
        uint16_t currentProductIndex;
        uint16_t currentRemoteIndex;
        uint16_t currentRemoteCommandIndex;
        uint16_t currentFileRemoteIndex;
        uint16_t currentFileRemoteCommandIndex;


        SelectionMode currentSelectedMode;
        Manufacturer currentSelectedManufacturer;
        Product currentSelectedProduct;
        Remote currentSelectedRemote;
        RemoteCommand currentSelectedRemoteCommand;
        FileRemote currentSelectedFileRemote;
        FileRemoteCommand currentSelectedFileRemoteCommand;
        std::string currentSelectedFilePath = "/";


        void handleModeSelection();
        void handleManufacturerSelection();
        void handleScanSelection();
        void handleProductSelection();
        void handleRemoteSelection();
        void handleRemoteCommandSelection();
        void handleFileRemoteSelection();
        void handleFileRemoteCommandSelection();

    };
}

#endif // APP_CONTROLLER_H
