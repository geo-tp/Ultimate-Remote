#ifndef LINUX_FRAMEBUFFER_VIEW_H
#define LINUX_FRAMEBUFFER_VIEW_H

#include <Interfaces/IDisplayView.h>

#include <cstdint>
#include <chrono>
#include <string>
#include <utility>
#include <vector>

namespace platform::linux {

class LinuxFramebufferView : public interfaces::IDisplayView {
public:
    explicit LinuxFramebufferView(const std::string& framebufferPath = "/dev/fb0");
    ~LinuxFramebufferView() override;

    void initialise() override;
    // Turn the actual backlight off where supported, otherwise use FBIOBLANK.
    bool setScreenBlanked(bool blank);
    void displayTopBar(const std::string& title, bool submenu = false, bool searchBar = false) override;
    void displaySelection(const std::vector<std::string>& selectionStrings,
                          uint16_t selectionIndex,
                          const std::vector<std::string>& selectionDescription = {}) override;
    void displayTransmissionFeedback(const std::string& command, bool success) override;
    void displayScanInfo() override;
    void displayScanRemote(std::string remoteName) override;
    void displayScanSelection(std::string manufacturerName) override;
    void displayScanCommand(std::string commandName) override;
    void displayScanOver() override;
    void displayStringPrompt(std::string stringDescription, std::string stringInput) override;
    void displayConfirmationPrompt(std::string stringDescription) override;
    void displayLoading() override;
    void displayIrFileInfo() override;
    void displayExitHold(int elapsedMilliseconds) override;
    void displayInfoPage(const std::string& title, const std::vector<std::string>& lines,
                         uint16_t page, uint16_t pageCount) override;
    void displayMessage(const std::string& title, const std::vector<std::string>& lines, bool busy = false) override;

    // Uses the same renderer without opening hardware; screenshots are RGB PPM.
    void initialisePreview(int pixelDepth = 32);
    bool savePreview(const std::string& path) const;

private:
    enum class TextStyle { Small, Body, Title };
    enum class Icon { Remote, Scan, Star, Folder, Search, Back, Check, Info, Settings };
    std::string framebufferPath;
    int fd = -1;
    uint8_t* buffer = nullptr;
    long screenSize = 0;
    long mappedSize = 0;
    std::vector<uint8_t> shadowBuffer;
    int width = 320;
    int height = 170;
    int lineLength = 0;
    int bitsPerPixel = 32;
    bool framebufferReady = false;
    bool screenBlanked = false;
    bool blankedViaBacklight = false;
    std::string backlightBrightnessPath;
    int previousBrightness = -1;
    std::string currentTitle;
    bool currentSubmenu = false;
    bool currentSearchBar = false;
    bool exitHoldVisible = false;
    std::string scanManufacturer;
    std::string scanRemote;
    unsigned scanStep = 0;
    bool wifiConnected = false;
    std::chrono::steady_clock::time_point lastWifiCheck {};
    void refreshWifiStatus();

    void clear(uint32_t color = 0x000000);
    void fillRect(int x, int y, int w, int h, uint32_t color);
    void roundedRect(int x, int y, int w, int h, int radius, uint32_t color);
    void line(int x1, int y1, int x2, int y2, uint32_t color, int thickness = 1);
    void drawIcon(Icon icon, int x, int y, int size, uint32_t color);
    void drawText(int x, int y, const std::string& text, uint32_t color, TextStyle style = TextStyle::Body);
    void drawCenteredText(int y, const std::string& text, uint32_t color, TextStyle style = TextStyle::Body);
    int textWidth(const std::string& text, TextStyle style = TextStyle::Body) const;
    std::string fitText(const std::string& text, int available, TextStyle style = TextStyle::Body) const;
    void renderStatusPage(const std::string& title, const std::vector<std::string>& lines,
                          Icon icon = Icon::Info, uint32_t accent = 0x88f0b3);
    void renderScan(const std::string& command);
    void renderHeader();
    void renderFooter(const std::vector<std::pair<std::string, std::string>>& hints);
    void renderCarousel(const std::vector<std::string>& selectionStrings,
                        uint16_t selectionIndex,
                        const std::vector<std::string>& selectionDescription, int slideOffset = 0);
    void renderFilePickerList(const std::vector<std::string>& selectionStrings,
                              uint16_t selectionIndex,
                              const std::vector<std::string>& selectionDescription);
    void putPixel(int x, int y, uint32_t color);
    uint32_t getPixel(int x, int y) const;
    void blendPixel(int x, int y, uint32_t color, uint8_t alpha);
    void flush();
};

}

#endif
