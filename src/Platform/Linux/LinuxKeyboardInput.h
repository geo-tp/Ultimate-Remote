#ifndef LINUX_KEYBOARD_INPUT_H
#define LINUX_KEYBOARD_INPUT_H

#include <Interfaces/IInputProvider.h>
#include <Inputs/InputKeys.h>
#include <array>
#include <chrono>
#include <functional>
#include <termios.h>
#include <vector>

namespace platform::linux {

class LinuxKeyboardInput : public interfaces::IInputProvider {
public:
    LinuxKeyboardInput();
    ~LinuxKeyboardInput() override;

    char handler() override;
    void setHomeMenuActive(bool active) override;
    int exitHoldMilliseconds() const override;
    bool supportsModifiedCommandKeys() const override { return true; }
    // Returns true when the platform really blanked/unblanked its display.
    void setScreenBlankingCallback(std::function<bool(bool)> callback);

private:
    termios originalTermios {};
    bool terminalConfigured = false;
    int originalInputFlags = -1;
    struct InputDevice {
        int fd;
        bool fnPressed = false;
        bool cardputerKeyboard = false;
        bool cardputerFnPressed = false;
        int pendingScanCode = -1;
        bool droppedEvents = false;
        bool escapePressed = false;
        std::array<bool, 4> heldDirections {};
        std::vector<unsigned short> wakeSuppressedKeys;  // ignore repeats until release
        std::vector<unsigned short> modifiedCommandKeys; // keep Fn commands one-shot until release
    };
    std::vector<InputDevice> inputDevices;
    std::array<char, 768> keyBindings {};
    bool debugInput = false;
    bool homeMenuActive = false;
    int exitHoldDevice = -1;
    std::chrono::steady_clock::time_point exitHoldStarted {};
    std::chrono::steady_clock::time_point lastEvdevEscape {};
    std::function<bool(bool)> screenBlankingCallback;
    std::chrono::steady_clock::time_point lastActivity = std::chrono::steady_clock::now();
    bool screenSleeping = false;
    void checkIdleScreen();
    bool registerActivity();  // true if this press was used only to wake
    char repeatingDirection = KEY_NONE;
    std::chrono::steady_clock::time_point nextDirectionRepeat {};
    char pollDirectionRepeat();
    void refreshDirectionRepeat();
    std::string terminalSequence;
    int pendingTerminalByte = -1;
    std::chrono::steady_clock::time_point terminalEscapeTime;

    void initKeyBindings();
    void configureTerminal();
    void restoreTerminal();
    void openInputDevices();
    void addInputDevice(int fd);
    void closeInputDevices();
    char readEvdevInput();
    char readTerminalInput();
    char terminalEscapeAction() const;
    void bindKey(int keyCode, char action);
    char mapLinuxKey(unsigned short code, bool fnPressed, char fnLetter = KEY_NONE) const;
    char mapLinuxTextKey(unsigned short code) const;
    [[noreturn]] void quitApplication();
};

}

#endif
