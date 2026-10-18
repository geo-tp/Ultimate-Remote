#ifndef ARDUINO
#include "LinuxFramebufferView.h"
#include "Assets/UiFont.h"
#include "Assets/UiIcons.h"
#include <Contexts/GlobalContext.h>

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <linux/fb.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

namespace platform::linux {

namespace {
constexpr uint32_t Background = 0x0b111a;
constexpr uint32_t Panel = 0x141f2d;
constexpr uint32_t Border = 0x27374a;
constexpr uint32_t Primary = 0x88f0b3;
constexpr uint32_t Text = 0xedf3fa;
constexpr uint32_t Muted = 0x95a6bb;
constexpr uint32_t Blue = 0x80baff;
constexpr uint32_t Amber = 0xffd18b;
constexpr uint32_t Violet = 0xbda7ff;
constexpr uint32_t Ink = 0x10241c;
constexpr int FooterHeight = 19;

std::string readableCommand(std::string text) {
    if (text.compare(0, 4, "KEY_") == 0) text.erase(0, 4);
    std::replace(text.begin(), text.end(), '_', ' ');
    return text;
}
}

LinuxFramebufferView::LinuxFramebufferView(const std::string& framebufferPath)
    : framebufferPath(framebufferPath) {}

LinuxFramebufferView::~LinuxFramebufferView() {
    if (screenBlanked) setScreenBlanked(false);
    if (fd >= 0) close(fd);
}

bool LinuxFramebufferView::setScreenBlanked(bool blank) {
    if (screenBlanked == blank) return true;
    if (fd < 0 || !framebufferReady) return false;

    // Cardputer Zero exposes its backlight as /sys/class/backlight/*/brightness
    // on supported device trees. Remember and restore the existing brightness.
    if (blank) {
        auto tryBacklight = [&](const std::string& path) -> bool {
            std::ifstream in(path);
            int brightness = -1;
            if (!(in >> brightness) || brightness <= 0) return false;
            std::ofstream out(path);
            if (!(out << "0\n")) return false;
            out.close();
            if (!out) return false;
            backlightBrightnessPath = path;
            previousBrightness = brightness;
            blankedViaBacklight = true;
            screenBlanked = true;
            return true;
        };
        if (tryBacklight("/sys/class/backlight/backlight_gpio/brightness")) return true;
        if (DIR* directory = opendir("/sys/class/backlight")) {
            while (dirent* entry = readdir(directory)) {
                if (entry->d_name[0] == '.') continue;
                if (tryBacklight(std::string("/sys/class/backlight/") +
                                 entry->d_name + "/brightness")) break;
            }
            closedir(directory);
            if (screenBlanked) return true;
        }
        // Not every driver exports its backlight: try framebuffer DPMS.
        if (ioctl(fd, FBIOBLANK, FB_BLANK_POWERDOWN) == 0) {
            blankedViaBacklight = false;
            screenBlanked = true;
            return true;
        }
        std::cerr << "[screen] Cannot blank display: no writable backlight or FBIOBLANK\n";
        return false;
    }
    bool restored = false;
    if (blankedViaBacklight) {
        std::ofstream out(backlightBrightnessPath);
        restored = static_cast<bool>(out << previousBrightness << "\n");
        out.close();
        restored = restored && static_cast<bool>(out);
    } else {
        restored = ioctl(fd, FBIOBLANK, FB_BLANK_UNBLANK) == 0;
    }
    if (restored) screenBlanked = false;
    else std::cerr << "[screen] Failed to restore display\n";
    return restored;
}

void LinuxFramebufferView::initialise() {
    fd = open(framebufferPath.c_str(), O_RDWR);
    if (fd < 0) {
        std::cerr << "[framebuffer] cannot open " << framebufferPath << ", using terminal fallback\n";
        return;
    }

    fb_var_screeninfo vinfo {};
    fb_fix_screeninfo finfo {};
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) != 0 || ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) != 0) {
        std::cerr << "[framebuffer] cannot query screen info\n";
        return;
    }

    width = static_cast<int>(vinfo.xres);
    height = static_cast<int>(vinfo.yres);
    bitsPerPixel = static_cast<int>(vinfo.bits_per_pixel);
    lineLength = static_cast<int>(finfo.line_length);
    screenSize = static_cast<long>(lineLength) * height;
    mappedSize = finfo.smem_len > 0 ? static_cast<long>(finfo.smem_len) : screenSize;
    if (width <= 0 || height <= 0 || lineLength <= 0 || screenSize <= 0 || mappedSize <= 0) {
        std::cerr << "[framebuffer] invalid geometry: "
                  << width << "x" << height
                  << ", lineLength=" << lineLength
                  << ", bpp=" << bitsPerPixel
                  << ", smemLen=" << finfo.smem_len << "\n";
        return;
    }

    if (mappedSize < screenSize) {
        int safeHeight = static_cast<int>(mappedSize / lineLength);
        if (safeHeight > 0 && safeHeight < height) {
            std::cerr << "[framebuffer] clamping height from " << height
                      << " to " << safeHeight
                      << " because smem_len is smaller than visible geometry\n";
            height = safeHeight;
            screenSize = static_cast<long>(lineLength) * height;
        }
    }

    if (bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32) {
        std::cerr << "[framebuffer] unsupported mode: " << bitsPerPixel << " bpp\n";
        return;
    }

    try {
        shadowBuffer.assign(static_cast<size_t>(screenSize), 0);
    } catch (...) {
        std::cerr << "[framebuffer] cannot allocate shadow buffer of " << screenSize << " bytes\n";
        return;
    }
    buffer = shadowBuffer.data();
    framebufferReady = buffer != nullptr;

    if (const char* debug = std::getenv("ULTIMATE_REMOTE_FB_DEBUG"); debug && debug[0] != '\0' && debug[0] != '0') {
        std::cerr << "[framebuffer] " << framebufferPath << " "
                  << width << "x" << height
                  << " lineLength=" << lineLength
                  << " bpp=" << bitsPerPixel
                  << " screenSize=" << screenSize
                  << " mappedSize=" << mappedSize
                  << " smemLen=" << finfo.smem_len << "\n";
    }

    clear();
    displayTopBar("Ultimate Remote");
}


void LinuxFramebufferView::initialisePreview(int pixelDepth) {
    if (pixelDepth != 16 && pixelDepth != 24 && pixelDepth != 32)
        throw std::invalid_argument("Preview depth must be 16, 24 or 32");
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    width = 320;
    height = 170;
    bitsPerPixel = pixelDepth;
    lineLength = width * (bitsPerPixel / 8);
    screenSize = mappedSize = lineLength * height;
    shadowBuffer.assign(static_cast<size_t>(screenSize), 0);
    buffer = shadowBuffer.data();
    framebufferReady = true;
    clear(Background);
}

bool LinuxFramebufferView::savePreview(const std::string& path) const {
    if (!framebufferReady) return false;
    std::ofstream file(path, std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint32_t color = getPixel(x, y);
            char rgb[] = {static_cast<char>(color >> 16), static_cast<char>(color >> 8),
                          static_cast<char>(color)};
            file.write(rgb, sizeof(rgb));
        }
    }
    return file.good();
}

void LinuxFramebufferView::displayTopBar(const std::string& title, bool submenu, bool searchBar) {
    currentTitle = title;
    currentSubmenu = submenu;
    currentSearchBar = searchBar;
    if (!framebufferReady) {
        std::cout << "\n== " << (submenu ? "< " : "") << title << (searchBar ? " [search]" : "") << " ==\n";
    }
    // Present the header together with the next content frame to avoid flicker.
}

void LinuxFramebufferView::displaySelection(const std::vector<std::string>& strings,
                                            uint16_t selectionIndex,
                                            const std::vector<std::string>& descriptions) {
    exitHoldVisible = false;
    if (!framebufferReady) {
        std::cout << "\033[2J\033[H";
        const size_t selected = strings.empty() ? 0 : std::min<size_t>(selectionIndex, strings.size() - 1);
        const size_t start = selected < 12 ? 0 : selected - 11;
        for (size_t i = start; i < strings.size() && i < start + 12; ++i) {
            std::cout << (i == selected ? "> " : "  ") << strings[i];
            if (i < descriptions.size()) std::cout << " - " << descriptions[i];
            std::cout << "\n";
        }
        std::cout.flush();
        return;
    }
    if (!currentSubmenu) {
        // Short directional easing when changing the horizontal mode.
        static int previous = -1;
        const int next = static_cast<int>(selectionIndex);
        const int count = static_cast<int>(strings.size());
        if (previous >= 0 && next != previous && count > 1) {
            const int direction = (next == (previous + 1) % count) ? 1 :
                                  (next == (previous + count - 1) % count) ? -1 : 0;
            if (direction) {
                for (int remaining : {30, 17, 7}) {
                    renderCarousel(strings, selectionIndex, descriptions, direction * remaining);
                    usleep(12000);
                }
            }
        }
        renderCarousel(strings, selectionIndex, descriptions);
        previous = next;
    } else renderFilePickerList(strings, selectionIndex, descriptions);
}

// Check Wi-Fi association at most once every three seconds. No network calls,
// just the Linux wireless interface carrier state (wlan0, wlp*, etc.).
void LinuxFramebufferView::refreshWifiStatus() {
    const auto now = std::chrono::steady_clock::now();
    if (lastWifiCheck != std::chrono::steady_clock::time_point{} &&
        now - lastWifiCheck < std::chrono::seconds(3)) return;
    lastWifiCheck = now;
    wifiConnected = false;
    DIR* directory = opendir("/sys/class/net");
    if (!directory) return;
    while (dirent* entry = readdir(directory)) {
        const std::string iface = entry->d_name;
        if (iface == "." || iface == "..") continue;
        const std::string base = "/sys/class/net/" + iface;
        // A wireless PHY symlink/dir is the reliable check; prefix is a fallback
        // for embedded kernels which expose wlan0 without a wireless directory.
        if (access((base + "/wireless").c_str(), F_OK) != 0 &&
            access((base + "/phy80211").c_str(), F_OK) != 0 &&
            iface.rfind("wlan", 0) != 0 && iface.rfind("wlp", 0) != 0) continue;
        std::ifstream carrier(base + "/carrier");
        int status = 0;
        if (carrier >> status && status == 1) {
            wifiConnected = true;
            break;
        }
    }
    closedir(directory);
}

void LinuxFramebufferView::renderHeader() {
    clear(Background);
    drawIcon(Icon::Remote, 8, 4, 18, Primary);
    drawText(33, 7, "ULTIMATE REMOTE", Text, TextStyle::Small);
    const std::string pin = "CFG  G" + std::to_string(contexts::GlobalContext::getInstance().getIrTxPin());
    const int badgeWidth = textWidth(pin, TextStyle::Small) + 14;
    refreshWifiStatus();
    // Compact Wi-Fi glyph immediately to the left of CFG Gxx.
    // Green = associated, muted = disconnected or unavailable.
    const int wx = width - badgeWidth - 26;
    const uint32_t wc = wifiConnected ? Primary : Muted;
    line(wx - 9, 9, wx - 4, 6, wc);
    line(wx - 4, 6, wx + 4, 6, wc);
    line(wx + 4, 6, wx + 9, 9, wc);
    line(wx - 5, 12, wx, 9, wc);
    line(wx, 9, wx + 5, 12, wc);
    roundedRect(wx - 1, 15, 3, 3, 1, wc);
    roundedRect(width - badgeWidth - 8, 4, badgeWidth, 17, 5, Panel);
    drawText(width - badgeWidth - 1, 7, pin, Primary, TextStyle::Small);
    fillRect(8, 25, width - 16, 1, Border);
}

void LinuxFramebufferView::renderFooter(const std::vector<std::pair<std::string, std::string>>& hints) {
    const int y = height - FooterHeight;
    fillRect(0, y, width, FooterHeight, Background);
    fillRect(8, y, width - 16, 1, Border);
    if (hints.empty()) return;
    int x = 8;
    const int slot = (width - 16) / static_cast<int>(hints.size());
    for (const auto& hint : hints) {
        const int keyWidth = textWidth(hint.first, TextStyle::Small) + 8;
        roundedRect(x, y + 4, keyWidth, 12, 3, Border);
        drawText(x + 4, y + 4, hint.first, Text, TextStyle::Small);
        drawText(x + keyWidth + 5, y + 4,
                 fitText(hint.second, slot - keyWidth - 10, TextStyle::Small),
                 Muted, TextStyle::Small);
        x += slot;
    }
}

void LinuxFramebufferView::renderCarousel(const std::vector<std::string>& strings,
                                          uint16_t selectionIndex,
                                          const std::vector<std::string>& descriptions, int slideOffset) {
    renderHeader();
    if (strings.empty()) {
        drawCenteredText(80, "No items", Muted);
        renderFooter({{"ESC", "Hold 3s"}});
        flush();
        return;
    }
    const int selected = std::min<int>(selectionIndex, strings.size() - 1);
    const int count = static_cast<int>(strings.size());
    struct Mode { Icon icon; uint32_t accent; std::string title; std::string subtitle; };
    auto mode = [&](int index) -> Mode {
        const std::string& label = strings[index];
        if (label.find("SCAN") != std::string::npos) return {Icon::Scan, Blue, "Scan", "Find a matching remote"};
        if (label.find("FAVOR") != std::string::npos) return {Icon::Star, Amber, "Favorites", "Saved remotes"};
        if (label.find("ONLINE IR") != std::string::npos)
            return {Icon::Folder, Violet, "Online IR", "Flipper Database"};
        if (label.find("IR BUILDER") != std::string::npos) return {Icon::Remote, Blue, "IR Builder", "Send custom IR codes"};
        if (label.find("SETTING") != std::string::npos) return {Icon::Settings, Amber, "Settings", "IR GPIO and screen timeout"};
        if (label == "INFO") return {Icon::Info, Blue, "Info", "About & quick guide"};
        return {Icon::Remote, Primary, "Remotes", "Preloaded / offline"};
    };

    const int cardWidth = 146;
    const int centerX = (width - cardWidth) / 2;
    for (int offset : {-1, 1, 0}) {
        int index = (selected + offset + count) % count;
        const Mode item = mode(index);
        const bool active = offset == 0;
        const int x = (active ? centerX : (offset < 0 ? 7 : width - 78)) + slideOffset;
        const int y = active ? 34 : 46;
        const int w = active ? cardWidth : 71;
        const int h = active ? 99 : 77;
        roundedRect(x, y, w, h, 11, active ? item.accent : Border);
        roundedRect(x + 1, y + 1, w - 2, h - 2, 10, active ? 0x1b2b37 : Panel);
        if (active) {
            roundedRect(x + w / 2 - 16, y, 32, 3, 1, item.accent);
            drawIcon(item.icon, x + (w - 34) / 2, y + 11, 34, item.accent);
            const auto label = fitText(item.title, w - 14, TextStyle::Title);
            drawText(x + (w - textWidth(label, TextStyle::Title)) / 2, y + 52,
                     label, Text, TextStyle::Title);
            const auto subtitle = fitText(item.subtitle, w - 12, TextStyle::Small);
            drawText(x + (w - textWidth(subtitle, TextStyle::Small)) / 2, y + 79,
                     subtitle, Muted, TextStyle::Small);
        } else {
            drawIcon(item.icon, x + (w - 25) / 2, y + 13, 25, Muted);
            const auto label = fitText(item.title, w - 8, TextStyle::Small);
            drawText(x + (w - textWidth(label, TextStyle::Small)) / 2, y + 52,
                     label, Muted, TextStyle::Small);
        }
    }
    const Mode current = mode(selected);
    const int dots = std::min(count, 12);
    const int dotsWidth = (dots - 1) * 12 + 14;
    int dotX = (width - dotsWidth) / 2;
    for (int i = 0; i < dots; ++i) {
        const bool active = i == selected;
        roundedRect(dotX, 141, active ? 14 : 5, 4, 2, active ? current.accent : Border);
        dotX += active ? 21 : 12;
    }
    renderFooter({{"Z / C", "Browse"}, {"OK", "Open"}, {"ESC", "Hold 3s"}});
    flush();
}

void LinuxFramebufferView::displayExitHold(int elapsedMilliseconds) {
    if (!framebufferReady) {
        if (!exitHoldVisible) std::cout << "Hold 3sec to return to home\n" << std::flush;
        exitHoldVisible = true;
        return;
    }
    exitHoldVisible = true;
    roundedRect(10, 42, width - 20, 96, 9, Border);
    roundedRect(11, 43, width - 22, 94, 8, Panel);
    roundedRect(width / 2 - 19, 51, 38, 16, 4, Border);
    drawCenteredText(54, "ESC", Text, TextStyle::Small);
    drawCenteredText(78, "Hold 3sec to return to home", Text);
    drawCenteredText(97, "Release to cancel", Muted, TextStyle::Small);
    const int barWidth = width - 60;
    roundedRect(30, 119, barWidth, 5, 2, Border);
    const int filled = barWidth * std::clamp(elapsedMilliseconds, 0, 3000) / 3000;
    if (filled > 0) roundedRect(30, 119, filled, 5, 2, Amber);
    flush();
}

void LinuxFramebufferView::renderFilePickerList(const std::vector<std::string>& strings,
                                                uint16_t selectionIndex,
                                                const std::vector<std::string>& descriptions) {
    renderHeader();
    const bool commands = !strings.empty() && descriptions.size() == strings.size() &&
        std::all_of(descriptions.begin(), descriptions.end(), [](const std::string& value) {
            return value.size() == 1 ||
                (value.size() == 4 && value.compare(0, 3, "Fn+") == 0 && value[3] >= 'A' && value[3] <= 'Z');
        });
    const int selected = strings.empty() ? 0 : std::min<int>(selectionIndex, strings.size() - 1);
    const std::string count = strings.empty() ? "0" : std::to_string(selected + 1) + " / " + std::to_string(strings.size());
    const int countWidth = textWidth(count, TextStyle::Small) + 12;
    const Icon contextIcon = currentSearchBar ? Icon::Search : (commands ? Icon::Remote : Icon::Back);
    drawIcon(contextIcon, 9, 33, 15, currentSearchBar ? Blue : Primary);
    std::string title = currentTitle.empty() ? (currentSearchBar ? "Type to filter" : "Choose an item") : currentTitle;
    if (commands && title.compare(0, 7, "Remote ") == 0) title.erase(0, 7);
    drawText(31, 32, fitText(title, width - countWidth - 51), Text);
    roundedRect(width - countWidth - 9, 31, countWidth, 18, 5, Panel);
    drawText(width - countWidth - 3, 35, count, Muted, TextStyle::Small);

    if (strings.empty()) {
        drawIcon(Icon::Search, width / 2 - 12, 64, 24, Muted);
        drawCenteredText(96, "Nothing here yet", Text);
        drawCenteredText(116, currentSearchBar ? "Try another search or go back" : "Go back to choose another item",
                         Muted, TextStyle::Small);
    } else {
        constexpr int top = 55;
        constexpr int rowStep = 23;
        const int visible = std::max(1, (height - FooterHeight - top) / rowStep);
        int start = std::max(0, selected - visible / 2);
        int end = std::min(static_cast<int>(strings.size()), start + visible);
        start = std::max(0, end - visible);
        for (int i = start; i < end; ++i) {
            const int y = top + (i - start) * rowStep;
            const bool active = i == selected;
            const uint32_t ink = active ? Ink : Text;
            roundedRect(8, y, width - 22, 21, 5, active ? Primary : Panel);
            if (active) {
                line(14, y + 8, 17, y + 10, ink);
                line(17, y + 10, 14, y + 12, ink);
            }
            int right = width - 22;
            if (i < static_cast<int>(descriptions.size()) && !descriptions[i].empty()) {
                // Long category tags must remain readable, especially "Universal".
                const int badgeTextWidth = descriptions[i] == "Universal" ? 90 :
                    descriptions[i] == "Online IR" ? 75 : 52;
                const std::string shortcut = fitText(descriptions[i], badgeTextWidth, TextStyle::Small);
                const int badge = textWidth(shortcut, TextStyle::Small) + 10;
                right = width - 22 - badge - 7;
                roundedRect(right + 6, y + 4, badge, 13, 3, active ? 0x63c58e : Border);
                drawText(right + 11, y + 5, shortcut, active ? Ink : Blue, TextStyle::Small);
            }
            const std::string label = commands ? readableCommand(strings[i]) : strings[i];
            drawText(23, y + 4, fitText(label, right - 23), ink);
        }
        if (strings.size() > static_cast<size_t>(visible)) {
            const int trackHeight = visible * rowStep - 2;
            const int thumb = std::max(9, trackHeight * visible / static_cast<int>(strings.size()));
            const int thumbY = top + (trackHeight - thumb) * start /
                std::max(1, static_cast<int>(strings.size()) - visible);
            roundedRect(width - 8, top, 3, trackHeight, 1, Panel);
            roundedRect(width - 8, thumbY, 3, thumb, 1, Muted);
        }
    }
    if (commands && currentTitle.compare(0, 7, "Remote ") == 0)
        renderFooter({{"X / F", "Move"}, {"OK", "Send"}, {"SPACE", "Fav"}, {"ESC", "Back"}});
    else if (commands) renderFooter({{"X / F", "Move"}, {"OK", "Send"}, {"ESC", "Back"}});
    else if (currentTitle == "Favorite remotes" || currentTitle.find("Favorite remotes | ") == 0)
        renderFooter({{"X / F", "Move"}, {"OK", "Open"}, {"SPACE", "Del"}, {"ESC", "Back"}});
    else if (currentTitle == "Online database" || currentTitle == "Downloaded" ||
             currentTitle == "Favorites" || currentSearchBar)
        renderFooter({{"X / F", "Move"}, {"OK", "Open"}, {"SPACE", "Fav"}, {"ESC", "Back"}});
    else renderFooter({{"X / F", "Move"}, {"OK", "Open"}, {"ESC", "Back"}});
    flush();
}

void LinuxFramebufferView::displayScanInfo() {
    if (!framebufferReady) {
        displayMessage("How scanning works", {
            "1. Choose the manufacturer of the device you want to control.",
            "2. The scanner tries every remote for that manufacturer in sequence, sending its commands one by one.",
            "3. If your device reacts, press SPACE to stop the scan and save the current remote to Favorites.",
            "ESC cancels the scan."});
        return;
    }
    renderHeader();
    drawIcon(Icon::Scan, 10, 35, 25, Blue);
    drawText(44, 40, "How scanning works", Text, TextStyle::Title);
    roundedRect(9, 64, width - 18, 81, 8, Panel);
    drawText(18, 69, "1. Choose the device manufacturer", Text, TextStyle::Small);
    drawText(18, 88, "2. All its remotes are tried in order", Text, TextStyle::Small);
    drawText(26, 101, "Each IR command is sent in turn", Muted, TextStyle::Small);
    drawText(18, 118, "3. Device reacts? Press SPACE", Text, TextStyle::Small);
    drawText(26, 131, "Stop scan & save remote to Favorites", Primary, TextStyle::Small);
    renderFooter({{"OK", "Start"}, {"ESC", "Back"}});
    flush();
}

void LinuxFramebufferView::displayTransmissionFeedback(const std::string& command, bool success) {
    if (!framebufferReady) {
        std::cout << (success ? "IR sent: " : "IR TX failed: ") << command << "\n";
        return;
    }
    const uint32_t accent = success ? Primary : Amber;
    roundedRect(32, 64, width - 64, 60, 8, accent);
    roundedRect(34, 66, width - 68, 56, 7, Panel);
    drawIcon(success ? Icon::Check : Icon::Info, 43, 83, 22, accent);
    drawText(73, 73, success ? "IR SENT" : "IR FAILED", accent, TextStyle::Title);
    drawText(73, 97, fitText(readableCommand(command), width - 119, TextStyle::Small), Text, TextStyle::Small);
    flush();
}

void LinuxFramebufferView::displayScanSelection(std::string manufacturerName) {
    scanManufacturer = manufacturerName;
    scanRemote.clear();
    scanStep = 0;
    renderScan("Preparing scan...");
}

void LinuxFramebufferView::displayScanRemote(std::string remoteName) {
    scanRemote = remoteName;
    renderScan("Testing remote...");
}

void LinuxFramebufferView::displayScanCommand(std::string commandName) {
    ++scanStep;
    renderScan(readableCommand(commandName));
}

void LinuxFramebufferView::renderScan(const std::string& command) {
    displayTopBar("Scanning", true);
    if (!framebufferReady) {
        std::cout << scanManufacturer << "\n" << scanRemote << "\n" << command << "\n";
        return;
    }
    renderHeader();
    drawText(10, 33, "Scanning", Text, TextStyle::Title);
    roundedRect(width - 58, 33, 48, 17, 5, 0x1b3048);
    drawText(width - 50, 36, "IR TX", Blue, TextStyle::Small);
    roundedRect(8, 58, width - 16, 70, 9, Panel);
    drawIcon(Icon::Scan, 18, 75, 33, Blue);
    drawText(63, 66, fitText(scanManufacturer, width - 78), Text);
    drawText(63, 86, fitText(scanRemote, width - 78, TextStyle::Small), Muted, TextStyle::Small);
    drawText(63, 106, fitText(command, width - 78), Blue);
    const int segment = (width - 24) / 12;
    for (int i = 0; i < 12; ++i)
        roundedRect(10 + i * segment, 137, segment - 4, 4, 2, i == static_cast<int>(scanStep % 12) ? Blue : Border);
    renderFooter({{"SPACE", "Save"}, {"ESC", "Stop"}});
    flush();
}

void LinuxFramebufferView::displayScanOver() {
    renderStatusPage("Scan complete", {"All profiles have been tested", "Go back to try another brand"}, Icon::Check, Primary);
}

void LinuxFramebufferView::displayStringPrompt(std::string description, std::string input) {
    displayTopBar(description, true);
    if (!framebufferReady) {
        std::cout << "> " << input << "\n";
        return;
    }
    renderHeader();
    const bool isFavoriteName = description == "Type a favorite name";
    drawIcon(isFavoriteName ? Icon::Star : Icon::Info, 12, 34, 20, isFavoriteName ? Amber : Blue);
    drawText(42, 34, isFavoriteName ? "Save favorite" : "Enter a value", Text, TextStyle::Title);
    drawText(12, 64, fitText(description, width - 24), Muted);
    roundedRect(10, 85, width - 20, 34, 7, Primary);
    roundedRect(11, 86, width - 22, 32, 6, Panel);
    const std::string visible = fitText(input, width - 44, TextStyle::Title);
    drawText(20, 94, visible, Text, TextStyle::Title);
    fillRect(22 + textWidth(visible, TextStyle::Title), 94, 2, 16, Primary);
    drawText(12, 131, isFavoriteName ? "Give this remote a short name" : "Edit the value and confirm", Muted, TextStyle::Small);
    renderFooter({{"OK", isFavoriteName ? "Save" : "Confirm"}, {"ESC", "Cancel"}});
    flush();
}

void LinuxFramebufferView::displayConfirmationPrompt(std::string description) {
    constexpr const char* deletePrefix = "Delete favorite: ";
    if (description.compare(0, std::strlen(deletePrefix), deletePrefix) == 0) {
        renderStatusPage("Delete favorite?", {description.substr(std::strlen(deletePrefix)),
                         "OK to delete / ESC to cancel"}, Icon::Info, Amber);
    } else {
        renderStatusPage("Confirm", {description, "OK to confirm, ESC to cancel"}, Icon::Info, Amber);
    }
}

void LinuxFramebufferView::displayLoading() {
    renderStatusPage("Loading", {"Reading your remotes...", "Please wait"}, Icon::Folder, Violet);
}

void LinuxFramebufferView::displayIrFileInfo() {
    renderStatusPage("Online IR", {"Remotes from Flipper-IRDB", "Downloaded + online"}, Icon::Folder, Violet);
}

void LinuxFramebufferView::displayInfoPage(const std::string& title, const std::vector<std::string>& lines,
                                          uint16_t page, uint16_t pageCount) {
    const std::string progress = std::to_string(page + 1) + " / " + std::to_string(pageCount);
    displayTopBar("Info: " + title, true);
    if (!framebufferReady) {
        std::cout << "Page " << progress << "\n";
        for (const auto& text : lines) std::cout << text << "\n";
        std::cout.flush();
        return;
    }
    renderHeader();
    drawIcon(Icon::Info, 10, 32, 18, Blue);
    drawText(36, 35, "ABOUT & QUICK GUIDE", Muted, TextStyle::Small);
    const int badgeWidth = textWidth(progress, TextStyle::Small) + 14;
    roundedRect(width - badgeWidth - 9, 32, badgeWidth, 18, 5, Panel);
    drawText(width - badgeWidth - 2, 36, progress, Blue, TextStyle::Small);
    roundedRect(8, 58, width - 16, 85, 8, Panel);
    drawText(18, 65, fitText(title, width - 36, TextStyle::Title), Blue, TextStyle::Title);
    int y = 91;
    for (const auto& text : lines) {
        if (y > 125) break;
        drawText(18, y, fitText(text, width - 36), Text);
        y += 17;
    }
    renderFooter({{"X / F", "Pages"}, {"OK", "Next"}, {"Z/ESC", "Back"}});
    flush();
}

void LinuxFramebufferView::displayMessage(const std::string& title, const std::vector<std::string>& lines, bool busy) {
    renderStatusPage(title, lines, Icon::Info, Blue);
    if (framebufferReady && busy) {
        renderFooter({{"ESC", "Cancel"}});
        flush();
    }
    std::cout.flush();
}

void LinuxFramebufferView::renderStatusPage(const std::string& title, const std::vector<std::string>& lines,
                                           Icon icon, uint32_t accent) {
    displayTopBar(title, true);
    if (!framebufferReady) {
        for (const auto& text : lines) std::cout << text << "\n";
        return;
    }
    renderHeader();
    drawIcon(icon, width / 2 - 14, 35, 28, accent);
    drawCenteredText(73, fitText(title, width - 24, TextStyle::Title), Text, TextStyle::Title);
    int y = 103;
    for (const auto& text : lines) {
        drawCenteredText(y, fitText(text, width - 24), Muted);
        y += 20;
        if (y > height - FooterHeight - 14) break;
    }
    if (title == "Loading") renderFooter({});
    else if (title == "Delete favorite?") renderFooter({{"OK", "Delete"}, {"ESC", "Cancel"}});
    else if (title == "Confirm") renderFooter({{"OK", "Confirm"}, {"ESC", "Cancel"}});
    else if (title == "Online IR") renderFooter({{"OK", "Browse files"}});
    else renderFooter({{"OK", "Continue"}, {"ESC", "Back"}});
    flush();
}

void LinuxFramebufferView::clear(uint32_t color) {
    fillRect(0, 0, width, height, color);
}

void LinuxFramebufferView::fillRect(int x, int y, int w, int h, uint32_t color) {
    for (int yy = std::max(0, y); yy < std::min(height, y + h); ++yy)
        for (int xx = std::max(0, x); xx < std::min(width, x + w); ++xx)
            putPixel(xx, yy, color);
}

void LinuxFramebufferView::roundedRect(int x, int y, int w, int h, int radius, uint32_t color) {
    radius = std::max(0, std::min(radius, std::min(w, h) / 2));
    for (int row = 0; row < h; ++row) {
        int inset = 0;
        const int distance = row < radius ? radius - row - 1 : (row >= h - radius ? row - (h - radius) : 0);
        if (distance > 0)
            inset = radius - static_cast<int>(std::sqrt(radius * radius - distance * distance));
        fillRect(x + inset, y + row, w - 2 * inset, 1, color);
    }
}

void LinuxFramebufferView::line(int x1, int y1, int x2, int y2, uint32_t color, int thickness) {
    const int dx = std::abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    const int dy = -std::abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int error = dx + dy;
    while (true) {
        fillRect(x1 - thickness / 2, y1 - thickness / 2, thickness, thickness, color);
        if (x1 == x2 && y1 == y2) break;
        int doubled = 2 * error;
        if (doubled >= dy) { error += dy; x1 += sx; }
        if (doubled <= dx) { error += dx; y1 += sy; }
    }
}

void LinuxFramebufferView::drawIcon(Icon icon, int x, int y, int size, uint32_t color) {
    if (size <= 0) return;
    // Masks are supersampled offline and blended only once, including joins.
    const auto& sizes = ui_icons::Icons[static_cast<unsigned>(icon)];
    const auto* mask = &sizes[0];
    for (const auto& candidate : sizes) {
        if (std::abs(candidate.size - size) < std::abs(mask->size - size)) mask = &candidate;
    }
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            uint8_t alpha;
            if (mask->size == size) {
                alpha = mask->pixels[row * size + col];
            } else {
                // Smooth fallback for future sizes not yet baked into the atlas.
                const float sourceX = std::clamp((col + 0.5f) * mask->size / size - 0.5f, 0.0f, static_cast<float>(mask->size - 1));
                const float sourceY = std::clamp((row + 0.5f) * mask->size / size - 0.5f, 0.0f, static_cast<float>(mask->size - 1));
                const int x0 = static_cast<int>(sourceX), y0 = static_cast<int>(sourceY);
                const int x1 = std::min(x0 + 1, mask->size - 1), y1 = std::min(y0 + 1, mask->size - 1);
                const float fx = sourceX - x0, fy = sourceY - y0;
                const float top = mask->pixels[y0 * mask->size + x0] * (1 - fx) + mask->pixels[y0 * mask->size + x1] * fx;
                const float bottom = mask->pixels[y1 * mask->size + x0] * (1 - fx) + mask->pixels[y1 * mask->size + x1] * fx;
                alpha = static_cast<uint8_t>(top * (1 - fy) + bottom * fy + 0.5f);
            }
            blendPixel(x + col, y + row, color, alpha);
        }
    }
}

uint32_t LinuxFramebufferView::getPixel(int x, int y) const {
    if (!framebufferReady || x < 0 || y < 0 || x >= width || y >= height) return 0;
    const int bytesPerPixel = bitsPerPixel / 8;
    const long offset = static_cast<long>(y) * lineLength + static_cast<long>(x) * bytesPerPixel;
    if (offset < 0 || offset + bytesPerPixel > screenSize) return 0;
    const uint8_t* location = buffer + offset;
    if (bitsPerPixel == 16) {
        uint16_t value;
        std::memcpy(&value, location, sizeof(value));
        return (((value >> 11) & 31) * 255 / 31 << 16) |
               (((value >> 5) & 63) * 255 / 63 << 8) | ((value & 31) * 255 / 31);
    }
    return (static_cast<uint32_t>(location[2]) << 16) | (location[1] << 8) | location[0];
}

void LinuxFramebufferView::blendPixel(int x, int y, uint32_t color, uint8_t alpha) {
    if (alpha == 0) return;
    if (alpha == 255) { putPixel(x, y, color); return; }
    uint32_t background = getPixel(x, y);
    uint32_t mixed = 0;
    for (int shift : {0, 8, 16}) {
        uint32_t channel = (((color >> shift) & 255) * alpha +
                            ((background >> shift) & 255) * (255 - alpha) + 127) / 255;
        mixed |= channel << shift;
    }
    putPixel(x, y, mixed);
}

void LinuxFramebufferView::putPixel(int x, int y, uint32_t color) {
    if (!framebufferReady || x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    const int bytesPerPixel = bitsPerPixel / 8;
    const long offset = static_cast<long>(y) * lineLength + static_cast<long>(x) * bytesPerPixel;
    if (offset < 0 || offset + bytesPerPixel > screenSize) {
        return;
    }

    uint8_t* location = buffer + offset;
    uint8_t r = static_cast<uint8_t>((color >> 16) & 0xff);
    uint8_t g = static_cast<uint8_t>((color >> 8) & 0xff);
    uint8_t b = static_cast<uint8_t>(color & 0xff);

    if (bitsPerPixel == 16) {
        uint16_t value = static_cast<uint16_t>(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3));
        std::memcpy(location, &value, sizeof(value));
    } else {
        location[0] = b;
        location[1] = g;
        location[2] = r;
        if (bitsPerPixel == 32) {
            location[3] = 0;
        }
    }
}

void LinuxFramebufferView::flush() {
    if (!framebufferReady || fd < 0 || shadowBuffer.empty()) {
        return;
    }

    ssize_t written = pwrite(fd, shadowBuffer.data(), shadowBuffer.size(), 0);
    if (written < 0 && (errno == EINVAL || errno == ESPIPE)) {
        if (lseek(fd, 0, SEEK_SET) >= 0) {
            written = write(fd, shadowBuffer.data(), shadowBuffer.size());
        }
    }

    if (written < 0) {
        std::cerr << "[framebuffer] write failed on " << framebufferPath << ": " << std::strerror(errno) << "\n";
        framebufferReady = false;
    } else if (static_cast<size_t>(written) != shadowBuffer.size()) {
        std::cerr << "[framebuffer] short write: " << written << "/" << shadowBuffer.size() << "\n";
    }
}


int LinuxFramebufferView::textWidth(const std::string& text, TextStyle style) const {
    const auto& font = style == TextStyle::Title ? ui_font::Title :
                       style == TextStyle::Small ? ui_font::Small : ui_font::Body;
    int total = 0;
    for (unsigned char ch : text) total += font.glyphs[(ch >= 32 && ch < 127 ? ch : '?') - 32].advance;
    return total;
}

std::string LinuxFramebufferView::fitText(const std::string& text, int available, TextStyle style) const {
    if (textWidth(text, style) <= available) return text;
    if (textWidth("...", style) > available) return "";
    std::string result = text;
    while (!result.empty() && textWidth(result + "...", style) > available) result.pop_back();
    return result + "...";
}

void LinuxFramebufferView::drawText(int x, int y, const std::string& text, uint32_t color, TextStyle style) {
    const auto& font = style == TextStyle::Title ? ui_font::Title :
                       style == TextStyle::Small ? ui_font::Small : ui_font::Body;
    for (unsigned char ch : text) {
        const auto& glyph = font.glyphs[(ch >= 32 && ch < 127 ? ch : '?') - 32];
        for (int row = 0; row < font.height; ++row)
            for (int col = 0; col < glyph.width; ++col)
                blendPixel(x + col, y + row, color, font.pixels[glyph.offset + row * glyph.width + col]);
        x += glyph.advance;
        if (x >= width) break;
    }
}

void LinuxFramebufferView::drawCenteredText(int y, const std::string& text, uint32_t color, TextStyle style) {
    drawText(std::max(4, (width - textWidth(text, style)) / 2), y, text, color, style);
}

}

#endif // !ARDUINO
