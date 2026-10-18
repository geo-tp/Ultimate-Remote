#ifdef ARDUINO
#include <M5Cardputer.h>
#include <Platform/Arduino/CardputerView.h>
#include <Platform/Arduino/CardputerInput.h>
#include <Dispatchers/AppDispatcher.h>
#include <Platform/Arduino/ArduinoInfraredSender.h>
#include <Platform/Arduino/CardputerDisplayAdapter.h>
#include <Platform/Arduino/ArduinoAppPlatform.h>

using namespace dispatchers;
namespace platform::arduino {
platform::arduino::CardputerDisplayAdapter display;
platform::arduino::CardputerInput input;
platform::arduino::ArduinoInfraredSender infraredSender;
platform::arduino::ArduinoAppPlatform platformActions;

AppDispatcher dispatcher(display, input, infraredSender, platformActions);

void applicationSetup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);

    dispatcher.setup();
    input.resetScreenTimer();
}

void applicationLoop() {
    dispatcher.run();
}

}
#endif
