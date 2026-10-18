#include "SettingsController.h"
#include <Inputs/InputKeys.h>
#include <Inputs/ScopedZBack.h>
#include <algorithm>
#include <iterator>
#include <cstdint>

namespace controllers {
void SettingsController::handle() {
    inputs::ScopedZBack zBack(input);
    uint16_t selected = 0;
    enum class Editing { None, Gpio, ScreenTimeout } editing = Editing::None;
    constexpr int timeoutValues[] = {0, 15, 30, 60, 120, 300};
    const std::vector<std::string> timeoutLabels = {
        "Off (never)", "15 seconds", "30 seconds (default)",
        "1 minute", "2 minutes", "5 minutes"
    };
    const auto timeoutName = [&]() {
        switch (globalContext.getScreenTimeoutSeconds()) {
            case 0: return std::string("Off");
            case 15: return std::string("15s");
            case 30: return std::string("30s");
            case 60: return std::string("1 min");
            case 120: return std::string("2 min");
            case 300: return std::string("5 min");
            default: return std::string("30s");
        }
    };
    // Do not repaint the LCD while waiting for a key. In particular, Settings
    // used to redraw both the top bar and the whole list on every idle poll.
    bool dirty = true;
    std::vector<std::string> gpioNames;
    while (true) {
        if (editing == Editing::None) {
            if (dirty) {
                const std::vector<std::string> names = {
                    "IR TX pin: GPIO " + std::to_string(globalContext.getIrTxPin()),
                    "Screen timeout: " + timeoutName(),
                    "Back to main menu"
                };
                display.displayTopBar("Settings", true);
                display.displaySelection(names, selected,
                                         {platform.irPinHint(), "Screen off when idle", "ESC"});
                dirty = false;
            }
            const char key = input.handler();
            if (key == KEY_RETURN || (key == KEY_OK && selected == 2)) break;
            if (key == KEY_ARROW_DOWN) {
                selected = (selected + 1) % 3;
                dirty = true;
            } else if (key == KEY_ARROW_UP) {
                selected = selected ? selected - 1 : 2;
                dirty = true;
            } else if (key == KEY_OK && selected == 0) {
                editing = Editing::Gpio;
                selected = static_cast<uint16_t>(std::clamp(globalContext.getIrTxPin(), 0, 53));
                dirty = true;
            } else if (key == KEY_OK && selected == 1) {
                editing = Editing::ScreenTimeout;
                auto it = std::find(std::begin(timeoutValues), std::end(timeoutValues),
                                    globalContext.getScreenTimeoutSeconds());
                selected = it == std::end(timeoutValues) ? 2 :
                    static_cast<uint16_t>(it - std::begin(timeoutValues));
                dirty = true;
            }
        } else if (editing == Editing::ScreenTimeout) {
            if (dirty) {
                display.displayTopBar("Screen timeout", true);
                display.displaySelection(timeoutLabels, selected);
                dirty = false;
            }
            const char key = input.handler();
            if (key == KEY_RETURN) {
                editing = Editing::None;
                selected = 1;
                dirty = true;
            } else if (key == KEY_ARROW_DOWN) {
                selected = (selected + 1) % timeoutLabels.size();
                dirty = true;
            } else if (key == KEY_ARROW_UP) {
                selected = selected ? selected - 1 : timeoutLabels.size() - 1;
                dirty = true;
            } else if (key == KEY_OK) {
                globalContext.setScreenTimeoutSeconds(timeoutValues[selected]);
                const bool saved = platform.saveScreenTimeout();
                if (!saved) {
                    display.displayMessage("Settings not saved", {"Could not write settings.conf", "OK to continue"});
                    while (true) {
                        const char response = input.handler();
                        if (response == KEY_OK || response == KEY_RETURN) break;
                    }
                }
                editing = Editing::None;
                selected = 1;
                dirty = true;
            }
        } else {
            if (dirty) {
                if (gpioNames.empty()) {
                    gpioNames.reserve(54);
                    for (int gpio = 0; gpio <= 53; ++gpio) {
                        gpioNames.push_back("GPIO " + std::to_string(gpio) +
                                            (gpio == ULTIMATE_REMOTE_IR_TX_PIN ? " (default)" : ""));
                    }
                }
                display.displayTopBar("Select IR TX GPIO", true);
                display.displaySelection(gpioNames, selected);
                dirty = false;
            }
            const char key = input.handler();
            if (key == KEY_RETURN) {
                editing = Editing::None;
                selected = 0;
                dirty = true;
            } else if (key == KEY_ARROW_DOWN) {
                selected = (selected + 1) % gpioNames.size();
                dirty = true;
            } else if (key == KEY_ARROW_UP) {
                selected = selected ? selected - 1 : gpioNames.size() - 1;
                dirty = true;
            } else if (key == KEY_OK) {
                globalContext.setIrTxPin(selected);
                platform.applyIrPin(selected, infraredService, display);
                while (true) {
                    const char confirmation = input.handler();
                    if (confirmation == KEY_OK || confirmation == KEY_RETURN) break;
                }
                editing = Editing::None;
                selected = 0;
                dirty = true;
            }
        }
    }
    selectionContext.setIsModeSelected(false);
}

}
