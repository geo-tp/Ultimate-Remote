#ifndef ARDUINO
#include <Contexts/GlobalContext.h>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>

namespace {
std::string settingsDir() {
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg && *xdg)
        return std::string(xdg) + "/ultimate-remote";
    if (const char* home = std::getenv("HOME"); home && *home)
        return std::string(home) + "/.config/ultimate-remote";
    return "";
}
}

void contexts::GlobalContext::loadSettings() {
    const std::string dir = settingsDir();
    if (dir.empty()) return;
    std::ifstream in(dir + "/settings.conf");
    std::string line;
    while (std::getline(in, line)) {
        try {
            if (line.compare(0, 10, "ir_tx_pin=") == 0) {
                const int value = std::stoi(line.substr(10));
                if (value >= 0 && value <= 53) setIrTxPin(value);
            } else if (line.compare(0, 23, "screen_timeout_seconds=") == 0) {
                setScreenTimeoutSeconds(std::stoi(line.substr(23)));
            }
        } catch (...) { /* Ignore broken user settings. */ }
    }
}

bool contexts::GlobalContext::saveSettings() const {
    const std::string dir = settingsDir();
    if (dir.empty()) return false;
    // First create ~/.config, then the application directory, honoring XDG paths.
    const auto slash = dir.rfind('/');
    if (slash == std::string::npos) return false;
    const std::string parent = dir.substr(0, slash);
    if (mkdir(parent.c_str(), 0700) != 0 && errno != EEXIST) return false;
    if (mkdir(dir.c_str(), 0700) != 0 && errno != EEXIST) return false;
    const std::string file = dir + "/settings.conf";
    const std::string temp = file + ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) return false;
    out << "ir_tx_pin=" << getIrTxPin() << "\n";
    out << "screen_timeout_seconds=" << getScreenTimeoutSeconds() << "\n";
    out.close();
    if (!out) return false;
    return std::rename(temp.c_str(), file.c_str()) == 0;
}
#endif
