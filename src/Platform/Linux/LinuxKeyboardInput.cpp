#ifndef ARDUINO
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

#ifdef KEY_OK
#undef KEY_OK
#endif

#include "LinuxKeyboardInput.h"
#include <Inputs/InputKeys.h>
#include <Inputs/CommandShortcuts.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Enums/SelectionModeEnum.h>

namespace platform::linux {

namespace {

// The tca8418c driver emits KEY_DVD for its Fn layer and already translates
// letters into media/navigation keycodes. See the pinned upstream keymap in
// docs/linux-ir.md. Generic keyboards still use KEY_FN plus plain letters.
char cardputerFnLetter(unsigned short code) {
    switch (code) {
        case KEY_PLAYPAUSE: return 'q';
        case KEY_PREVIOUSSONG: return 'w';
        case KEY_NEXTSONG: return 'e';
        case KEY_BRIGHTNESSDOWN: return 'u';
        case KEY_BRIGHTNESSUP: return 'i';
        case KEY_F11: return 'o';
        case KEY_F12: return 'p';
        case KEY_MUTE: return 'a';
        case KEY_VOLUMEDOWN: return 's';
        case KEY_VOLUMEUP: return 'd';
        case KEY_UP: return 'f';
        case KEY_HELP: return 'h';
        case KEY_SYSRQ: return 'j';
        case KEY_HOME: return 'k';
        case KEY_PAGEUP: return 'l';
        case KEY_LEFT: return 'z';
        case KEY_DOWN: return 'x';
        case KEY_RIGHT: return 'c';
        case KEY_INSERT: return 'b';
        case KEY_END: return 'n';
        case KEY_PAGEDOWN: return 'm';
        default: return KEY_NONE;
    }
}

char cardputerScanLetter(int scan) {
    // Matrix coordinates from MSC_SCAN identify the physical letter, including
    // firmware versions whose horizontal arrow keycodes are reversed.
    if (scan >= 0x10 && scan <= 0x19) return "qwertyuiop"[scan - 0x10];
    if (scan >= 0x20 && scan <= 0x28) return "asdfghjkl"[scan - 0x20];
    if (scan >= 0x32 && scan <= 0x38) return "zxcvbnm"[scan - 0x32];
    return KEY_NONE;
}

}

LinuxKeyboardInput::LinuxKeyboardInput() {
    const char* debug = std::getenv("ULTIMATE_REMOTE_INPUT_DEBUG");
    debugInput = debug && debug[0] != '\0' && debug[0] != '0';

    initKeyBindings();
    configureTerminal();
    openInputDevices();
}

LinuxKeyboardInput::~LinuxKeyboardInput() {
    if (screenSleeping && screenBlankingCallback) screenBlankingCallback(false);
    closeInputDevices();
    restoreTerminal();
}

void LinuxKeyboardInput::setScreenBlankingCallback(std::function<bool(bool)> callback) {
    screenBlankingCallback = std::move(callback);
    lastActivity = std::chrono::steady_clock::now();
}

void LinuxKeyboardInput::checkIdleScreen() {
    if (exitHoldDevice >= 0) {
        lastActivity = std::chrono::steady_clock::now();
        return;
    }
    if (!screenBlankingCallback) return;
    const auto& settings = contexts::GlobalContext::getInstance();
    const auto& selection = contexts::SelectionContext::getInstance();
    // Scanning transmits IR automatically: never interrupt it for inactivity.
    const bool scanning = selection.getIsModeSelected() &&
        selection.getCurrentSelectedMode() == enums::SelectionModeEnum::SCAN;
    const int timeout = settings.getScreenTimeoutSeconds();
    if (scanning || timeout <= 0) {
        if (screenSleeping && screenBlankingCallback(false)) screenSleeping = false;
        lastActivity = std::chrono::steady_clock::now();
        return;
    }
    if (screenSleeping) return;
    if (std::chrono::steady_clock::now() - lastActivity >= std::chrono::seconds(timeout)) {
        if (screenBlankingCallback(true)) screenSleeping = true;
        else lastActivity = std::chrono::steady_clock::now(); // avoid busy-loop retry
    }
}

bool LinuxKeyboardInput::registerActivity() {
    lastActivity = std::chrono::steady_clock::now();
    if (!screenSleeping) return false;
    // The first physical key wakes the screen but never activates its UI action.
    if (screenBlankingCallback && screenBlankingCallback(false)) screenSleeping = false;
    repeatingDirection = KEY_NONE;
    return true;
}

void LinuxKeyboardInput::configureTerminal() {
    originalInputFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (originalInputFlags >= 0) fcntl(STDIN_FILENO, F_SETFL, originalInputFlags | O_NONBLOCK);
    if (!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &originalTermios) != 0) {
        return;
    }

    termios raw = originalTermios;
    raw.c_lflag &= static_cast<unsigned>(~(ICANON | ECHO));
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    terminalConfigured = true;
}

void LinuxKeyboardInput::restoreTerminal() {
    if (originalInputFlags >= 0) fcntl(STDIN_FILENO, F_SETFL, originalInputFlags);
    if (terminalConfigured) {
        tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
    }
}

void LinuxKeyboardInput::setHomeMenuActive(bool active) {
    homeMenuActive = active;
    // A press that began in a submenu can never become an exit gesture.
    exitHoldDevice = -1;
}

int LinuxKeyboardInput::exitHoldMilliseconds() const {
    if (!homeMenuActive || exitHoldDevice < 0) return -1;
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - exitHoldStarted).count());
}

char LinuxKeyboardInput::handler() {
    checkIdleScreen();
    char key = readEvdevInput();
    // Read pending releases/disconnects before evaluating the deadline. No
    // kernel autorepeat is needed: a continuous physical press is sufficient.
    if (exitHoldMilliseconds() >= 3000) quitApplication();
    if (key != KEY_NONE) {
        return key;
    }

    key = readTerminalInput();
    if (key == KEY_NONE) key = pollDirectionRepeat();
    if (key != KEY_NONE) lastActivity = std::chrono::steady_clock::now();
    if (key == KEY_NONE) {
        usleep(10000);
    }

    return key;
}

void LinuxKeyboardInput::initKeyBindings() {
    keyBindings.fill(KEY_NONE);

    bindKey(KEY_F, KEY_ARROW_UP);
    bindKey(KEY_X, KEY_ARROW_DOWN);
    bindKey(KEY_UP, KEY_ARROW_UP);
    bindKey(KEY_DOWN, KEY_ARROW_DOWN);
    // The Cardputer Zero D-pad reports horizontal directions reversed via evdev.
    bindKey(KEY_LEFT, KEY_ARROW_RIGHT);
    bindKey(KEY_RIGHT, KEY_ARROW_LEFT);

    bindKey(KEY_ENTER, KEY_OK);
    bindKey(KEY_KPENTER, KEY_OK);
}

void LinuxKeyboardInput::bindKey(int keyCode, char action) {
    if (keyCode >= 0 && static_cast<size_t>(keyCode) < keyBindings.size()) {
        keyBindings[static_cast<size_t>(keyCode)] = action;
    }
}

void LinuxKeyboardInput::openInputDevices() {
    const char* configuredDevice = std::getenv("ULTIMATE_REMOTE_INPUT_DEVICE");
    if (configuredDevice && configuredDevice[0] != '\0') {
        int fd = open(configuredDevice, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            addInputDevice(fd);
            std::cout << "input: opened " << configuredDevice << "\n";
            if (debugInput) {
                std::cout << "input: using configured device only\n";
            }
        } else {
            std::cerr << "input: cannot open " << configuredDevice << ": " << std::strerror(errno) << "\n";
        }
        return;
    }

    for (int i = 0; i < 32; ++i) {
        char path[64];
        std::snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            addInputDevice(fd);
            std::cout << "input: opened " << path << "\n";
        } else {
            if (errno != ENOENT && errno != ENODEV) {
                std::cerr << "input: cannot open " << path << ": " << std::strerror(errno) << "\n";
            }
        }
    }

    if (inputDevices.empty()) {
        std::cerr << "input: no /dev/input/event* devices opened; falling back to stdin only\n";
        std::cerr << "input: set ULTIMATE_REMOTE_INPUT_DEVICE=/dev/input/eventX or adjust permissions\n";
    }
}

void LinuxKeyboardInput::addInputDevice(int fd) {
    InputDevice device {};
    device.fd = fd;
    char name[256] {};
    if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), name) >= 0) {
        std::string lower = name;
        for (char& c : lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        device.cardputerKeyboard = lower.find("tca8418") != std::string::npos ||
                                   lower.find("cardputerzero") != std::string::npos ||
                                   lower.find("cardputer zero") != std::string::npos;
    }
    if (device.cardputerKeyboard) {
        std::cerr << "input: Cardputer Zero keyboard detected (" << name << "), decoding hardware Fn layer\n";
        // Keep Fn media keys from also adjusting system volume/playing audio
        // or arriving a second time as console escape sequences. Closing the
        // descriptor on exit releases this app-only grab automatically.
        if (ioctl(fd, EVIOCGRAB, 1) < 0)
            std::cerr << "input: cannot grab Cardputer keyboard: " << std::strerror(errno) << "\n";
    }
    unsigned char keys[(KEY_MAX + 8) / 8] {};
    if (ioctl(fd, EVIOCGKEY(sizeof(keys)), keys) >= 0) {
        device.fnPressed = (keys[KEY_FN / 8] & (1u << (KEY_FN % 8))) != 0;
        device.cardputerFnPressed = device.cardputerKeyboard &&
            (keys[KEY_DVD / 8] & (1u << (KEY_DVD % 8))) != 0;
    }
    inputDevices.push_back(std::move(device));
}

void LinuxKeyboardInput::closeInputDevices() {
    for (const auto& device : inputDevices) {
        if (device.fd >= 0) {
            close(device.fd);
        }
    }
    inputDevices.clear();
}

char LinuxKeyboardInput::readEvdevInput() {
    for (auto& device : inputDevices) {
        input_event event {};
        ssize_t bytesRead;
        while ((bytesRead = read(device.fd, &event, sizeof(event))) == sizeof(event)) {
            if (debugInput) {
                std::cerr << "input: event fd=" << device.fd
                          << " type=" << event.type
                          << " code=" << event.code
                          << " value=" << event.value << "\n";
            }

            if (event.type == EV_SYN && event.code == SYN_DROPPED) {
                device.fnPressed = false;
                device.cardputerFnPressed = false;
                device.pendingScanCode = -1;
                device.heldDirections.fill(false);
                device.wakeSuppressedKeys.clear();
                device.modifiedCommandKeys.clear();
                device.escapePressed = false;
                if (exitHoldDevice == device.fd) exitHoldDevice = -1;
                refreshDirectionRepeat();
                device.droppedEvents = true;
            }
            if (device.droppedEvents) {
                if (event.type == EV_SYN && event.code == SYN_REPORT) {
                    unsigned char keys[(KEY_MAX + 8) / 8] {};
                    if (ioctl(device.fd, EVIOCGKEY(sizeof(keys)), keys) >= 0) {
                        device.fnPressed = (keys[KEY_FN / 8] & (1u << (KEY_FN % 8))) != 0;
                        device.cardputerFnPressed = device.cardputerKeyboard &&
                            (keys[KEY_DVD / 8] & (1u << (KEY_DVD % 8))) != 0;
                        device.escapePressed = (keys[KEY_ESC / 8] & (1u << (KEY_ESC % 8))) != 0;
                        device.heldDirections = {{
                            (keys[KEY_F / 8] & (1u << (KEY_F % 8))) != 0 || (keys[KEY_UP / 8] & (1u << (KEY_UP % 8))) != 0,
                            (keys[KEY_X / 8] & (1u << (KEY_X % 8))) != 0 || (keys[KEY_DOWN / 8] & (1u << (KEY_DOWN % 8))) != 0,
                            (keys[KEY_LEFT / 8] & (1u << (KEY_LEFT % 8))) != 0,
                            (keys[KEY_RIGHT / 8] & (1u << (KEY_RIGHT % 8))) != 0
                        }};
                        refreshDirectionRepeat();
                    }
                    device.droppedEvents = false;
                }
                continue;
            }
            if (event.type == EV_MSC && event.code == MSC_SCAN) {
                device.pendingScanCode = event.value;
                continue;
            }
            if (event.type == EV_SYN && event.code == SYN_REPORT) device.pendingScanCode = -1;
            if (event.type != EV_KEY) {
                continue;
            }
            const int scanCode = device.pendingScanCode;
            device.pendingScanCode = -1;
            // Also recognize the driver's physical Fn signature when a custom
            // device name hides its identity (or when replaying recorded events).
            if (event.code == KEY_DVD && scanCode == 0x42) device.cardputerKeyboard = true;
            const bool fnEvent = event.code == KEY_FN ||
                (device.cardputerKeyboard && event.code == KEY_DVD);
            if (event.code == KEY_ESC) lastEvdevEscape = std::chrono::steady_clock::now();
            // The key that woke the display is ignored until its release;
            // otherwise Linux's autorepeat could accidentally trigger actions.
            auto wakeKey = std::find(device.wakeSuppressedKeys.begin(),
                                     device.wakeSuppressedKeys.end(), event.code);
            if (wakeKey != device.wakeSuppressedKeys.end()) {
                if (event.value == 0) {
                    device.wakeSuppressedKeys.erase(wakeKey);
                    if (event.code == KEY_FN) device.fnPressed = false;
                    if (fnEvent && event.code == KEY_DVD) device.cardputerFnPressed = false;
                }
                continue;
            }
            // Physical input (including Fn) wakes the display before it can
            // trigger Back, Send, a favorite toggle, or another action.
            if ((event.value == 1 || event.value == 2) && registerActivity()) {
                if (event.value == 1) device.wakeSuppressedKeys.push_back(event.code);
                if (event.code == KEY_FN) device.fnPressed = true;
                if (fnEvent && event.code == KEY_DVD) device.cardputerFnPressed = true;
                return KEY_NONE;
            }
            if (fnEvent) {
                if (event.code == KEY_FN) device.fnPressed = event.value != 0;
                else device.cardputerFnPressed = event.value != 0;
                if ((textInputEnabled || literalTextInputEnabled || commandInputEnabled) && event.value != 0)
                    repeatingDirection = KEY_NONE;
                continue;
            }
            if (event.code == KEY_ESC) {
                if (event.value == 0) {
                    device.escapePressed = false;
                    if (exitHoldDevice == device.fd) exitHoldDevice = -1;
                } else if (event.value == 1 && !device.escapePressed) {
                    device.escapePressed = true;
                    if (homeMenuActive) {
                        if (exitHoldDevice < 0) {
                            exitHoldDevice = device.fd;
                            exitHoldStarted = std::chrono::steady_clock::now();
                        }
                    } else {
                        return KEY_RETURN;
                    }
                }
                // Repeats never cause another Back or restart the hold timer.
                continue;
            }
            // Evdev autorepeat is replaced with one consistent repeat timer for
            // both the physical D-pad and the F/X navigation shortcuts.
            int directionIndex = -1;
            if (event.code == KEY_F || event.code == KEY_UP) directionIndex = 0;
            else if (event.code == KEY_X || event.code == KEY_DOWN) directionIndex = 1;
            else if (event.code == KEY_LEFT) directionIndex = 2;
            else if (event.code == KEY_RIGHT) directionIndex = 3;
            if (directionIndex >= 0) {
                device.heldDirections[static_cast<size_t>(directionIndex)] = event.value != 0;
                if (event.value == 0) refreshDirectionRepeat();
            }
            auto commandKey = std::find(device.modifiedCommandKeys.begin(),
                                        device.modifiedCommandKeys.end(), event.code);
            if (commandKey != device.modifiedCommandKeys.end()) {
                if (event.value == 0) device.modifiedCommandKeys.erase(commandKey);
                continue;
            }
            if (event.value == 0) continue;
            if (event.value != 1 && event.value != 2) continue;
            // Only real text is allowed to use kernel autorepeat. SPACE toggles
            // favorites; holding it must never toggle them on/off repeatedly.
            // ENTER/Z are also one-shot navigation actions.
            if (event.value == 2 && (directionIndex >= 0 || event.code == KEY_SPACE ||
                                     event.code == KEY_ENTER || event.code == KEY_KPENTER ||
                                     event.code == KEY_TAB || event.code == KEY_Z)) continue;

            bool fnHeld = false;
            for (const auto& source : inputDevices) fnHeld |= source.fnPressed;
            fnHeld |= device.cardputerFnPressed;
            char fnLetter = KEY_NONE;
            if (device.cardputerKeyboard) {
                fnLetter = cardputerFnLetter(event.code);
                // A translated key itself proves Fn was active at key-down,
                // even if the layer was released or enabled before app startup.
                if (fnLetter != KEY_NONE) {
                    const char physicalLetter = cardputerScanLetter(scanCode);
                    if (physicalLetter != KEY_NONE) fnLetter = physicalLetter;
                    fnHeld = true;
                }
            }
            const char modifiedLetter = fnLetter != KEY_NONE ? fnLetter : mapLinuxTextKey(event.code);
            if (fnHeld && (textInputEnabled || literalTextInputEnabled || commandInputEnabled))
                repeatingDirection = KEY_NONE;
            if (commandInputEnabled && fnHeld &&
                inputs::modifiedCommandKey(modifiedLetter) != KEY_NONE) {
                if (event.value != 1) continue;
                device.modifiedCommandKeys.push_back(event.code);
            }
            char key = mapLinuxKey(event.code, fnHeld, fnLetter);
            if (key == KEY_NONE) {
                key = mapLinuxTextKey(static_cast<unsigned short>(event.code));
            }
            if (debugInput && key == KEY_NONE) {
                std::cerr << "input: unmapped key code=" << event.code << "\n";
            }
            if (key != KEY_NONE) {
                // While exiting, drain all devices (especially Esc releases)
                // before checking the deadline; other actions cannot open a menu.
                if (exitHoldDevice >= 0) continue;
                if (directionIndex >= 0 &&
                    (key == KEY_ARROW_UP || key == KEY_ARROW_DOWN ||
                     key == KEY_ARROW_LEFT || key == KEY_ARROW_RIGHT)) {
                    repeatingDirection = key;
                    nextDirectionRepeat = std::chrono::steady_clock::now() + std::chrono::milliseconds(350);
                }
                if (debugInput) {
                    std::cerr << "input: mapped code=" << event.code
                              << " to app key=" << static_cast<int>(key) << "\n";
                }
                return key;
            }
        }
        if (bytesRead == 0 || (bytesRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)) {
            // Losing the device is not evidence that Esc is still held.
            device.escapePressed = false;
            device.fnPressed = false;
            device.cardputerFnPressed = false;
            device.pendingScanCode = -1;
            device.heldDirections.fill(false);
            device.modifiedCommandKeys.clear();
            if (exitHoldDevice == device.fd) exitHoldDevice = -1;
            refreshDirectionRepeat();
        }
    }

    return KEY_NONE;
}

char LinuxKeyboardInput::pollDirectionRepeat() {
    if (repeatingDirection == KEY_NONE ||
        std::chrono::steady_clock::now() < nextDirectionRepeat) return KEY_NONE;
    nextDirectionRepeat = std::chrono::steady_clock::now() + std::chrono::milliseconds(85);
    return repeatingDirection;
}

void LinuxKeyboardInput::refreshDirectionRepeat() {
    // The key-release event must stop repetition, even if the UI is idle.
    const auto isHeld = [this](char direction) {
        for (const auto& device : inputDevices) {
            const auto& d = device.heldDirections;
            if (direction == KEY_ARROW_UP && d[0]) return true;
            if (direction == KEY_ARROW_DOWN && d[1]) return true;
            // Physical left/right are reversed on the Cardputer Zero.
            if (direction == KEY_ARROW_RIGHT && d[2]) return true;
            if (direction == KEY_ARROW_LEFT && d[3]) return true;
        }
        return false;
    };
    if (!isHeld(repeatingDirection)) repeatingDirection = KEY_NONE;
}

char LinuxKeyboardInput::terminalEscapeAction() const {
    // A console may echo the same physical key that evdev already delivered.
    // Do not let that duplicate navigate back a second time or act on wake-up.
    for (const auto& device : inputDevices) if (device.escapePressed) return KEY_NONE;
    if (std::chrono::steady_clock::now() - lastEvdevEscape < std::chrono::milliseconds(150))
        return KEY_NONE;
    return KEY_RETURN;
}

char LinuxKeyboardInput::readTerminalInput() {
    if (!terminalSequence.empty() && std::chrono::steady_clock::now() - terminalEscapeTime >
            std::chrono::milliseconds(50)) {
        const bool escape = terminalSequence.size() == 1;
        terminalSequence.clear();
        if (escape) return terminalEscapeAction();
    }
    unsigned char c = 0;
    if (pendingTerminalByte >= 0) {
        c = static_cast<unsigned char>(pendingTerminalByte);
        pendingTerminalByte = -1;
    } else if (read(STDIN_FILENO, &c, 1) != 1) {
        return KEY_NONE;
    }

    if (registerActivity()) {
        terminalSequence.clear();
        return KEY_NONE;
    }
    if (!terminalSequence.empty()) {
        if (terminalSequence.size() == 1) {
            // Terminals do not expose Fn: Alt+letter provides the same text override.
            if (textInputEnabled && c == ' ') {
                terminalSequence.clear();
                return KEY_FILTER_SPACE;
            }
            if ((textInputEnabled || literalTextInputEnabled || commandInputEnabled) && c != 'O' && std::isalpha(c)) {
                terminalSequence.clear();
                if (commandInputEnabled && !literalTextInputEnabled && !textInputEnabled)
                    return inputs::modifiedCommandKey(static_cast<char>(std::tolower(c)));
                return static_cast<char>(std::tolower(c));
            }
            if (c != '[' && c != 'O') {
                terminalSequence.clear();
                pendingTerminalByte = c;
                return terminalEscapeAction();
            }
        }
        terminalSequence += static_cast<char>(c);
        if (terminalSequence.size() > 2 && c >= 0x40 && c <= 0x7e) {
            const bool deleteKey = terminalSequence == "\x1b[3~";
            terminalSequence.clear();
            switch (c) {
                case 'A': return KEY_ARROW_UP;
                case 'B': return KEY_ARROW_DOWN;
                case 'C': return KEY_ARROW_RIGHT;
                case 'D': return KEY_ARROW_LEFT;
                default: return deleteKey ? KEY_DEL : KEY_NONE;
            }
        }
        if (terminalSequence.size() > 16) terminalSequence.clear();
        return KEY_NONE;
    }
    if (c == 0x1b) {
        terminalSequence = "\x1b";
        terminalEscapeTime = std::chrono::steady_clock::now();
        return KEY_NONE;
    }

    if (literalTextInputEnabled && std::isalpha(c)) return static_cast<char>(std::tolower(c));
    switch (static_cast<char>(std::tolower(c))) {
        case 'f': return KEY_ARROW_UP;
        case 'x': return KEY_ARROW_DOWN;
        case 'z': return zBackEnabled ? KEY_RETURN : 'z';
        case '\t': return KEY_NONE;
        case '\x7f': return KEY_DEL;
        case '\n':
        case '\r': return KEY_OK;
        default: break;
    }

    return static_cast<char>(std::tolower(c));
}

char LinuxKeyboardInput::mapLinuxKey(unsigned short code, bool fnPressed, char fnLetter) const {
    const char letter = fnLetter != KEY_NONE ? fnLetter : mapLinuxTextKey(code);
    // Name prompts are literal: even navigation keys F/X/Z are ordinary letters.
    if (literalTextInputEnabled) {
        if (letter >= 'a' && letter <= 'z') return letter;
    }
    if (textInputEnabled && fnPressed) {
        if (code == KEY_SPACE) return KEY_FILTER_SPACE;
        if (letter >= 'a' && letter <= 'z') return letter;
    }
    if (commandInputEnabled && fnPressed) {
        const char command = inputs::modifiedCommandKey(letter);
        if (command != KEY_NONE) return command;
    }
    if (zBackEnabled && code == KEY_Z) return KEY_RETURN;
    if (static_cast<size_t>(code) >= keyBindings.size()) {
        return KEY_NONE;
    }

    return keyBindings[static_cast<size_t>(code)];
}

char LinuxKeyboardInput::mapLinuxTextKey(unsigned short code) const {
    switch (code) {
        case KEY_A: return 'a';
        case KEY_B: return 'b';
        case KEY_C: return 'c';
        case KEY_D: return 'd';
        case KEY_E: return 'e';
        case KEY_F: return 'f';
        case KEY_G: return 'g';
        case KEY_H: return 'h';
        case KEY_I: return 'i';
        case KEY_J: return 'j';
        case KEY_K: return 'k';
        case KEY_L: return 'l';
        case KEY_M: return 'm';
        case KEY_N: return 'n';
        case KEY_O: return 'o';
        case KEY_P: return 'p';
        case KEY_Q: return 'q';
        case KEY_R: return 'r';
        case KEY_S: return 's';
        case KEY_T: return 't';
        case KEY_U: return 'u';
        case KEY_V: return 'v';
        case KEY_W: return 'w';
        case KEY_X: return 'x';
        case KEY_Y: return 'y';
        case KEY_Z: return 'z';
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';
        case KEY_0: return '0';
        case KEY_SPACE: return ' ';
        case KEY_MINUS: return '-';
        case KEY_BACKSPACE:
        case KEY_DELETE: return KEY_DEL;
        default: break;
    }

    return KEY_NONE;
}

void LinuxKeyboardInput::quitApplication() {
    std::cerr << "input: ESC held for 3 seconds on home, exiting\n";
    if (screenSleeping && screenBlankingCallback) screenBlankingCallback(false);
    closeInputDevices();
    restoreTerminal();
    std::exit(0);
}

}

#endif // !ARDUINO
