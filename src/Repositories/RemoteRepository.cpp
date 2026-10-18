#include "RemoteRepository.h"

#include <algorithm>
#include <cstdio>

namespace repositories {

const Remote RemoteRepository::emptyRemote = {"", nullptr, 0};
const FileRemote RemoteRepository::emptyFileRemote = {"", {}};

const RemoteCommand RemoteRepository::emptyRemoteCommand = {"", ProtocolEnum(0), -1, -1, -1};
const FileRemoteCommand RemoteRepository::emptyFileRemoteCommand = {};

RemoteRepository::RemoteRepository(interfaces::IFavoritesStorage& favoriteStorage)
    : storage(favoriteStorage) {}

RemoteRepository::~RemoteRepository() {
}

std::vector<FavoriteRemote> RemoteRepository::getUserFavoriteRemotes() {
    return storage.load();
}

std::vector<FavoriteRemote> RemoteRepository::getDefaultFavoriteRemotes() {
    return defaultFavoriteRemotes;
}

std::vector<FavoriteRemote> RemoteRepository::getAllFavoriteRemotes() {
    std::vector<FavoriteRemote> userFavorites = getUserFavoriteRemotes();
    std::vector<FavoriteRemote> defaultFavorites = getDefaultFavoriteRemotes();

    // Combiner les deux
    std::vector<FavoriteRemote> allFavorites = userFavorites;
    // Ajouter en premier les remotes users et les remotes par défaut à la fin
    allFavorites.insert(allFavorites.end(), defaultFavorites.begin(), defaultFavorites.end());

    return allFavorites;
}

std::vector<FavoriteRemote> RemoteRepository::saveUserFavoriteRemote(
    const std::string& manufacturerName, const std::string& productName,
    const std::string& fileName, const std::string& favoriteName) {
    auto favorites = storage.load();
    if (manufacturerName.empty() || favorites.size() >= static_cast<size_t>(context.getMaxFavorites() / 2))
        return favorites;
    FavoriteRemote favorite {};
    std::snprintf(favorite.manufacturerName, sizeof(favorite.manufacturerName), "%s", manufacturerName.c_str());
    std::snprintf(favorite.productName, sizeof(favorite.productName), "%s", productName.c_str());
    std::snprintf(favorite.remoteFileName, sizeof(favorite.remoteFileName), "%s", fileName.c_str());
    std::snprintf(favorite.favoriteName, sizeof(favorite.favoriteName), "%s", favoriteName.c_str());
    storage.append(favorite);
    return storage.load();
}

std::vector<FavoriteRemote> RemoteRepository::deleteUserFavoriteRemote(
    const std::string& favoriteName, const std::string& productName) {
    storage.remove(favoriteName, productName);
    return storage.load();
}

Remote RemoteRepository::getEmptyRemote() {
    return emptyRemote;
}

RemoteCommand RemoteRepository::getEmptyRemoteCommand() {
    return emptyRemoteCommand;
}

FileRemote RemoteRepository::getEmptyFileRemote() {
    return emptyFileRemote;
}

FileRemoteCommand RemoteRepository::getEmptyFileRemoteCommand() {
    return emptyFileRemoteCommand;
}


}
