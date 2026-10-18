#pragma once

namespace interfaces { class IDisplayView; class IInputProvider; }
namespace controllers { class RemoteController; class CommandController; }
namespace services {
class FileService;
class RemoteService;
class ProductService;
class ManufacturerService;
}

namespace platform {
struct AppPlatformDependencies {
    interfaces::IDisplayView& display;
    interfaces::IInputProvider& input;
    controllers::RemoteController& remotes;
    controllers::CommandController& commands;
    services::FileService& files;
    services::RemoteService& remoteService;
    services::ProductService& products;
    services::ManufacturerService& manufacturers;
};

}
