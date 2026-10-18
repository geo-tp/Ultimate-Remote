#ifdef ARDUINO
#include "ArduinoFavoritesStorage.h"
#include <Preferences.h>
#include <Arduino.h>
#include <algorithm>
#include <cstdio>

namespace platform::arduino {
namespace {
constexpr const char* Namespace = "favoriteRemotes";
constexpr const char* Count = "favoriteCount";
}
std::vector<models::FavoriteRemote> ArduinoFavoritesStorage::load() {
    Preferences preferences;
    std::vector<models::FavoriteRemote> results;
    if (!preferences.begin(Namespace, true)) return results;
    const size_t count = preferences.getUInt(Count, 0);
    for (size_t i = 0; i < count; ++i) {
        String key = "favorite" + String(i);
        const String value = preferences.getString(key.c_str(), "");
        models::FavoriteRemote favorite {};
        if (std::sscanf(value.c_str(), "%23[^|]|%29[^|]|%13[^|]|%13[^|]",
                        favorite.manufacturerName, favorite.productName,
                        favorite.remoteFileName, favorite.favoriteName) == 4)
            results.push_back(favorite);
    }
    preferences.end();
    std::reverse(results.begin(), results.end());
    return results;
}
bool ArduinoFavoritesStorage::append(const models::FavoriteRemote& favorite) {
    Preferences preferences;
    if (!preferences.begin(Namespace, false)) return false;
    const size_t count = preferences.getUInt(Count, 0);
    const String key = "favorite" + String(count);
    char value[128];
    std::snprintf(value, sizeof(value), "%s|%s|%s|%s", favorite.manufacturerName,
                  favorite.productName, favorite.remoteFileName, favorite.favoriteName);
    const bool saved = preferences.putString(key.c_str(), value) > 0;
    if (saved) preferences.putUInt(Count, count + 1);
    preferences.end();
    return saved;
}
bool ArduinoFavoritesStorage::remove(const std::string& name, const std::string& product) {
    auto favorites = load();
    favorites.erase(std::remove_if(favorites.begin(), favorites.end(), [&](const auto& favorite) {
        return name == favorite.favoriteName && product == favorite.productName;
    }), favorites.end());
    std::reverse(favorites.begin(), favorites.end());
    Preferences preferences;
    if (!preferences.begin(Namespace, false)) return false;
    preferences.clear();
    preferences.putUInt(Count, favorites.size());
    bool success = true;
    for (size_t i = 0; i < favorites.size(); ++i) {
        char value[128];
        std::snprintf(value, sizeof(value), "%s|%s|%s|%s", favorites[i].manufacturerName,
                      favorites[i].productName, favorites[i].remoteFileName, favorites[i].favoriteName);
        const String key = "favorite" + String(i);
        if (!preferences.putString(key.c_str(), value)) success = false;
    }
    preferences.end();
    return success;
}
}
#endif
