#pragma once
#include <Interfaces/IAppPlatform.h>
namespace controllers {
class FileBrowserController {
public:
    explicit FileBrowserController(interfaces::IAppPlatform& platform) : platform(platform) {}
    void handle();
private:
    interfaces::IAppPlatform& platform;
};
}
