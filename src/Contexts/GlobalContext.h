#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <string>

#ifndef ULTIMATE_REMOTE_IR_TX_PIN
#define ULTIMATE_REMOTE_IR_TX_PIN 12
#endif

namespace contexts {

class GlobalContext {
public:
    // Singleton instance accessor
    static GlobalContext& getInstance();

    // Getters and setters
    std::string getAppName() const;
    void setAppName(const std::string& appName);

    int getMaxFavorites() const;
    void setMaxFavorites(int maxFavorites);

    int getProtocolCount() const;
    void setProtocolCount(int protocolCount);

    int getManufacturerCount() const;
    void setManufacturerCount(int manufacturerCount);

    int getMaxInputCharCount() const;
    void setMaxInputCharCount(int maxInputCharCount);

    int getLedPin() const;
    void setLedPin(int ledPin);

    int getIrTxPin() const;

    int getScreenTimeoutSeconds() const;
    void setScreenTimeoutSeconds(int seconds);
#ifndef ARDUINO
    void loadSettings();
    bool saveSettings() const;
#endif
    void setIrTxPin(int irTxPin);

    std::string getLircDevicePath() const;
    void setLircDevicePath(const std::string& lircDevicePath);

    int getSdCardCSPin() const;
    void setSdCardCSPin(int sdCardCSPin);

    int getSdCardMISOPin() const;
    void setSdCardMISOPin(int sdCardMISOPin);

    int getSdCardMOSIPin() const;
    void setSdCardMOSIPin(int sdCardMOSIPin);

    int getSdCardCLKPin() const;
    void setSdCardCLKPin(int sdCardCLKPin);

    int getFileCacheLimit() const;
    void setFileCacheLimit(int fileCacheLimit);

    int getFileCountLimit() const;
    void setFileCountLimit(int fileCountLimit);

private:
    // Private constructor to prevent instantiation
    GlobalContext();

    std::string appName = "Ultimate Remote";
    int maxFavorites = 80;
    int protocolCount = 83;
    int manufacturerCount = 636;
    int maxInputCharCount = 14;
    int ledPin = 21;
    int irTxPin = ULTIMATE_REMOTE_IR_TX_PIN;
    int screenTimeoutSeconds = 30;  // 0 = never turn off the display
    std::string lircDevicePath = "/dev/lirc0";
    int sdCardCSPin = 12;
    int sdCardMISOPin = 39;
    int sdCardMOSIPin = 14;
    int sdCardCLKPin = 40;
    int fileCacheLimit = 20;
    int fileCountLimit = 512;
};

} // namespace contexts

#endif // GLOBAL_CONTEXT_H
