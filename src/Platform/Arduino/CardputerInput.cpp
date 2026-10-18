#ifdef ARDUINO
#include "CardputerInput.h"
#include <Inputs/InputKeys.h>
#include <Arduino.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Enums/SelectionModeEnum.h>

namespace platform::arduino {
namespace {
constexpr uint32_t repeatDelayMs = 400;
constexpr uint32_t repeatIntervalMs = 80;
constexpr uint32_t fastRepeatAfterMs = 2000;
constexpr uint32_t fastRepeatIntervalMs = 40;

// Keep the classic Cardputer/ADV layout: comma is Back and slash is OK,
// semicolon/dot are vertical navigation. Fn+the printed arrow keys also
// produces directional actions on firmware/library versions that expose Fn.
// The physical ` key is Back/ESC directly (no Fn required).
// Some M5Cardputer versions also expose the separate ESC flag; support both.
template <typename State>
auto reportsEscape(const State& state, int) -> decltype(state.esc, bool()) {
    return state.esc;
}
template <typename State>
bool reportsEscape(const State&, long) { return false; }

char normalizeCardputerKey(char key, bool homeMenu) {
    if (key >= 'A' && key <= 'Z') return static_cast<char>(key - 'A' + 'a');
    switch (key) {
        case '+': return '='; // Volume+ shortcut is stored on the base '=' key.
        case '{': return '[';
        case '}': return ']';
        case '<': return homeMenu ? KEY_ARROW_LEFT : KEY_RETURN;
        case '>': return KEY_ARROW_DOWN; // Shift+'.': retain the Down action.
        case ':': return KEY_ARROW_UP;
        case '?': return homeMenu ? KEY_ARROW_RIGHT : KEY_OK;
        case ';': return KEY_ARROW_UP;
        case '.': return KEY_ARROW_DOWN;
        case ',': return homeMenu ? KEY_ARROW_LEFT : KEY_RETURN;
        case '/': return homeMenu ? KEY_ARROW_RIGHT : KEY_OK;
        default: return key;
    }
}
}

void CardputerInput::resetScreenTimer() {
    lastActivityAt = millis();
}

char CardputerInput::handler() {
    M5Cardputer.update();

    const uint32_t now = millis();
    const bool buttonDown = M5Cardputer.BtnA.isPressed();
    const bool keyboardDown = M5Cardputer.Keyboard.isPressed();
    const bool physicalInput = buttonDown || keyboardDown;

    const int timeout = contexts::GlobalContext::getInstance().getScreenTimeoutSeconds();
    const auto& selection = contexts::SelectionContext::getInstance();
    // Keep the screen on during automatic IR scanning, as on Linux.
    const bool scanning = selection.getIsModeSelected() &&
        selection.getCurrentSelectedMode() == enums::SelectionModeEnum::SCAN;

    if (scanning || timeout <= 0) {
        if (screenSleeping) {
            M5Cardputer.Display.setBrightness(awakeBrightness);
            screenSleeping = false;
        }
        lastActivityAt = now;
    } else if (!screenSleeping && !physicalInput &&
               now - lastActivityAt >= static_cast<uint32_t>(timeout) * 1000UL) {
        // Disable the backlight, not the LCD contents; the previous UI remains
        // available instantly on wake, without a full redraw.
        awakeBrightness = M5Cardputer.Display.getBrightness();
        if (awakeBrightness == 0) awakeBrightness = 128;
        M5Cardputer.Display.setBrightness(0);
        screenSleeping = true;
    }

    if (screenSleeping) {
        if (physicalInput) {
            M5Cardputer.Display.setBrightness(awakeBrightness);
            screenSleeping = false;
            suppressWakeInput = true;
            lastActivityAt = now;
            lastKey = KEY_NONE;
            goButtonLatched = false;
        }
        // The first press wakes the display but must not select a menu item.
        delay(5);
        return KEY_NONE;
    }

    if (suppressWakeInput) {
        // Ignore the entire wake press, including key repeat, until release.
        if (physicalInput) {
            lastActivityAt = now;
            delay(5);
            return KEY_NONE;
        }
        suppressWakeInput = false;
        lastKey = KEY_NONE;
        goButtonLatched = false;
    }
    if (physicalInput) lastActivityAt = now;

    // The GO button is a back action, emitted only once per physical press.
    if (!buttonDown) goButtonLatched = false;
    if (buttonDown && !goButtonLatched) {
        goButtonLatched = true;
        return KEY_RETURN;
    }

    if (!keyboardDown) {
        lastKey = KEY_NONE;
        delay(5);
        return KEY_NONE;
    }

    // Inspect held keys on every poll; isChange() only tracks the key count.
    const Keyboard_Class::KeysState& status = M5Cardputer.Keyboard.keysState();
    char key = KEY_NONE;

    // Back/ESC is the physical ` key alone, in every view including filters.
    // Accept a library-reported ESC too. On the home carousel comma remains
    // Previous; in submenus it is a second Back key.
    if (!homeMenuActive && M5Cardputer.Keyboard.isKeyPressed(','))
        key = KEY_RETURN;
    else if (M5Cardputer.Keyboard.isKeyPressed('`') ||
        (!status.word.empty() && (status.word.front() == '`' ||
                                  status.word.front() == '\x1b')) ||
        reportsEscape(status, 0))
        key = KEY_RETURN;
    else if (status.enter) key = KEY_OK;
    else if (status.del) key = KEY_DEL;
    else if (status.fn && !literalTextInputEnabled) {
        // The M5Cardputer library deliberately leaves Fn handling to apps.
        // Fn+Space adds a filter separator; all letters can already be typed
        // directly in search fields, without a Fn modifier.
        if (textInputEnabled && M5Cardputer.Keyboard.isKeyPressed(' '))
            key = KEY_FILTER_SPACE;
        else if (textInputEnabled && !status.word.empty() &&
                 ((status.word.front() >= 'a' && status.word.front() <= 'z') ||
                  (status.word.front() >= 'A' && status.word.front() <= 'Z')))
            key = normalizeCardputerKey(status.word.front(), homeMenuActive);
        else if (M5Cardputer.Keyboard.isKeyPressed(';')) key = KEY_ARROW_UP;
        else if (M5Cardputer.Keyboard.isKeyPressed('.')) key = KEY_ARROW_DOWN;
        else if (M5Cardputer.Keyboard.isKeyPressed(',')) key = homeMenuActive ? KEY_ARROW_LEFT : KEY_RETURN;
        else if (M5Cardputer.Keyboard.isKeyPressed('/')) key = homeMenuActive ? KEY_ARROW_RIGHT : KEY_OK;
    }

    if (key == KEY_NONE && !status.word.empty()) {
        const char character = status.word.front();
        if (literalTextInputEnabled) {
            // Name entry is literal, including F/X/Z, except the ESP32 Back key.
            if (character == ',' || character == '<') key = KEY_RETURN;
            else key = (character >= 'A' && character <= 'Z')
                ? static_cast<char>(character - 'A' + 'a') : character;
        } else if (textInputEnabled && status.fn) {
            key = (character >= 'A' && character <= 'Z')
                ? static_cast<char>(character - 'A' + 'a') : character;
        } else {
            key = normalizeCardputerKey(character, homeMenuActive);
            // F and X must remain available to filtering and IR command
            // shortcuts: Cardputer uses ; and . for vertical navigation.
            // Keep Z as Back only outside search and command input contexts.
            if (key == 'z' && zBackEnabled && !textInputEnabled && !commandInputEnabled)
                key = KEY_RETURN;
        }
    }

    // Some library versions expose only the Fn key flags, not their character
    // in word. These physical tests preserve the historical navigation.
    if (key == KEY_NONE && !literalTextInputEnabled) {
        if (M5Cardputer.Keyboard.isKeyPressed(',')) key = homeMenuActive ? KEY_ARROW_LEFT : KEY_RETURN;
        else if (M5Cardputer.Keyboard.isKeyPressed('/')) key = homeMenuActive ? KEY_ARROW_RIGHT : KEY_OK;
        else if (M5Cardputer.Keyboard.isKeyPressed(';')) key = KEY_ARROW_UP;
        else if (M5Cardputer.Keyboard.isKeyPressed('.')) key = KEY_ARROW_DOWN;
    }

    if (key != lastKey) {
        lastKey = key;
        keyPressedAt = now;
        lastRepeatAt = now;
        return key;
    }

    // Repeat navigation only: comma/slash are Back/OK outside the home menu,
    // and punctuation in a literal name prompt must remain a single input.
    const bool repeatable = !literalTextInputEnabled &&
        (key == KEY_ARROW_UP || key == KEY_ARROW_DOWN ||
         (homeMenuActive && (key == KEY_ARROW_LEFT || key == KEY_ARROW_RIGHT)));
    const uint32_t heldMs = now - keyPressedAt;
    const uint32_t intervalMs = heldMs >= fastRepeatAfterMs
        ? fastRepeatIntervalMs : repeatIntervalMs;
    if (repeatable && heldMs >= repeatDelayMs && now - lastRepeatAt >= intervalMs) {
        lastRepeatAt = now;
        return key;
    }

    // Fn/Shift changes keep the same normalized direction and repeat timer.
    delay(5);
    return KEY_NONE;
}
}

#endif // ARDUINO
