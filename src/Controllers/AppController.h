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
#include <Services/ProtocolService.h>
#include <Repositories/ManufacturerRepository.h>

#include <Selections/ManufacturerSelection.h>
#include <Selections/ModeSelection.h>
#include <Selections/ProductSelection.h>
#include <Selections/RemoteCommandSelection.h>
#include <Selections/RemoteSelection.h>
#include <Selections/ScanSelection.h>

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
        ProductService productService;
        RemoteService remoteService;
        ProtocolService protocolService;

        // TODO: Find why database is imported 2x when using service
        // ManufacturerService manufacturerService;
        ManufacturerRepository manufacturerService; // using Repo for now

        bool isModeSelected;
        bool isManufacturerSelected;
        bool isProductSelected;
        bool isRemoteSelected;

        uint16_t currentManufacturerIndex;
        uint16_t currentProductIndex;
        uint16_t currentRemoteIndex;
        uint16_t currentRemoteCommandIndex;

        SelectionMode currentSelectedMode;
        Manufacturer currentSelectedManufacturer;
        Product currentSelectedProduct;
        Remote currentSelectedRemote;
        RemoteCommand currentSelectedRemoteCommand;

        void handleModeSelection();
        void handleManufacturerSelection();
        void handleScanSelection();
        void handleProductSelection();
        void handleRemoteSelection();
        void handleRemoteCommandSelection();
    };
}

#endif // APP_CONTROLLER_H
