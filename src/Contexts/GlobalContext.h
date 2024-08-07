#ifndef GLOBALCONTEXT_H
#define GLOBALCONTEXT_H

#include <string>

namespace contexts {

class GlobalContext {
public:
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

private:
    GlobalContext();

    std::string appName;
    int maxFavorites;
    int protocolCount;
    int manufacturerCount;
    int maxInputCharCount;
};

}

#endif // GLOBALCONTEXT_H
