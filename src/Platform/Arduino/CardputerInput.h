#ifndef CARDPUTER_KEYBOARD_INPUT_H
#define CARDPUTER_KEYBOARD_INPUT_H
#include <M5Cardputer.h>
#include <Inputs/InputKeys.h>
#include <Interfaces/IInputProvider.h>

namespace platform::arduino {
class CardputerInput : public interfaces::IInputProvider {
public:
    bool homeMenuUsesLetterNavigation() const override { return false; }
    bool refreshFavoritesAfterDelete() const override { return true; }
    bool allowLeadingFilterSpace() const override { return false; }
    bool isNavigationReservedKey(char key) const override { return key == '`'; }
    char handler() override;
    // Start the idle timer after the display and app have been initialized.
    void resetScreenTimer();
    void setHomeMenuActive(bool active) override { homeMenuActive = active; }
private:
    bool goButtonLatched = false;
    bool homeMenuActive = false;
    char lastKey = KEY_NONE;
    uint32_t keyPressedAt = 0;
    uint32_t lastRepeatAt = 0;
    uint32_t lastActivityAt = 0;
    uint8_t awakeBrightness = 128;
    bool screenSleeping = false;
    bool suppressWakeInput = false;
};
}
#endif
