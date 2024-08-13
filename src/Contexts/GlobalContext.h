#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <string>

namespace contexts {

class GlobalContext {
public:
    // Singleton instance accessor
    static GlobalContext& getInstance();

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
    void setIrTxPin(int irTxPin);

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

    // Members for existing configurations
    std::string appName;
    int maxFavorites;
    int protocolCount;
    int manufacturerCount;
    int maxInputCharCount;

    // Led
    int ledPin;

    // Infrared
    int irTxPin;

    // SD card
    int sdCardCSPin;
    int sdCardMISOPin;
    int sdCardMOSIPin;
    int sdCardCLKPin;

    // Cache limits
    int fileCacheLimit;
    int fileCountLimit;
};

} // namespace contexts

#endif // GLOBAL_CONTEXT_H
