#ifndef ARDUINO
#include "LinuxFavoritesStorage.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace platform::linux {
namespace {
std::filesystem::path stateFile() {
    const char* dir = std::getenv("ULTIMATE_REMOTE_STATE_DIR");
    return std::filesystem::path(dir ? dir : ".") / "favoriteRemotes.txt";
}
}
std::vector<models::FavoriteRemote> LinuxFavoritesStorage::load() {
    std::vector<models::FavoriteRemote> results;
    std::ifstream file(stateFile());
    std::string line;
    while (std::getline(file, line)) {
        models::FavoriteRemote favorite {};
        if (std::sscanf(line.c_str(), "%23[^|]|%29[^|]|%13[^|]|%13[^|]",
                        favorite.manufacturerName, favorite.productName,
                        favorite.remoteFileName, favorite.favoriteName) == 4)
            results.push_back(favorite);
    }
    std::reverse(results.begin(), results.end());
    return results;
}
bool LinuxFavoritesStorage::append(const models::FavoriteRemote& favorite) {
    std::error_code error;
    std::filesystem::create_directories(stateFile().parent_path(), error);
    if (error) return false;
    std::ofstream file(stateFile(), std::ios::app);
    file << favorite.manufacturerName << '|' << favorite.productName << '|'
         << favorite.remoteFileName << '|' << favorite.favoriteName << '\n';
    return file.good();
}
bool LinuxFavoritesStorage::remove(const std::string& name, const std::string& product) {
    auto favorites = load(); // Newest first, disk order is oldest first.
    favorites.erase(std::remove_if(favorites.begin(), favorites.end(), [&](const auto& favorite) {
        return favorite.favoriteName == name && favorite.productName == product;
    }), favorites.end());
    std::reverse(favorites.begin(), favorites.end());
    std::error_code error;
    std::filesystem::create_directories(stateFile().parent_path(), error);
    if (error) return false;
    std::ofstream file(stateFile(), std::ios::trunc);
    for (const auto& favorite : favorites)
        file << favorite.manufacturerName << '|' << favorite.productName << '|'
             << favorite.remoteFileName << '|' << favorite.favoriteName << '\n';
    return file.good();
}
}
#endif
