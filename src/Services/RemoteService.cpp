#include "RemoteService.h"

namespace services {

RemoteService::RemoteService(RemoteRepository& repository)
    : remoteRepository(repository) {}


RemoteService::~RemoteService() {}

Remote RemoteService::getRemote(const Product& product, const std::string& remoteFileName) {
    for (size_t i = 0; i < product.remoteCount; ++i) {
        Remote remote;
        memcpy_P(&remote, &product.remotes[i], sizeof(Remote));
        if (strcmp(remote.fileName, remoteFileName.c_str()) == 0) {
            return remote;
        }
    }
    return remoteRepository.getEmptyRemote();
}

std::vector<Remote> RemoteService::getRemotes(const Product& product) {
    std::vector<Remote> remotes;
    for (size_t i = 0; i < product.remoteCount; ++i) {
        Remote remote;
        memcpy_P(&remote, &product.remotes[i], sizeof(Remote));
        remotes.push_back(remote);
    }
    return remotes;
}


Remote RemoteService::getEmptyRemote() {
    return remoteRepository.getEmptyRemote();
}

RemoteCommand RemoteService::getEmptyRemoteCommand() {
    return remoteRepository.getEmptyRemoteCommand();
}

FileRemote RemoteService::getEmptyFileRemote() {
    return remoteRepository.getEmptyFileRemote();
}

FileRemoteCommand RemoteService::getEmptyFileRemoteCommand() {
    return remoteRepository.getEmptyFileRemoteCommand();
}

std::vector<RemoteCommand> RemoteService::getRemoteCommands(const Remote& remote) {
    std::vector<RemoteCommand> commands;
    for (size_t i = 0; i < remote.commandCount; ++i) {
        RemoteCommand command;
        memcpy_P(&command, &remote.commands[i], sizeof(RemoteCommand));
        commands.push_back(command);
    }
    return commands;
}

Remote RemoteService::getFavoriteRemoteToRemote(Manufacturer manufacturer, Product product, const FavoriteRemote& favoriteRemote) {
    // FavoriteRemote Struct to Remote Struct
    if (manufacturer.name[0] != '\0') {
        if (product.name[0] != '\0') {
            for (size_t i = 0; i < product.remoteCount; ++i) {
                if (strcmp(product.remotes[i].fileName, favoriteRemote.remoteFileName) == 0) {
                    Remote remote = product.remotes[i];

                    // Copier le favoriteName comme remote.fileName
                    strncpy(remote.fileName, favoriteRemote.favoriteName, sizeof(remote.fileName) - 1);
                    remote.fileName[sizeof(remote.fileName) - 1] = '\0';

                    return remote;
                }
            }
        }
    }
    return remoteRepository.getEmptyRemote();
}

std::vector<Remote> RemoteService::getFavoriteRemotesToRemotes(Manufacturer manufacturer, Product product, std::vector<FavoriteRemote> favoriteRemotes) {
    std::vector<Remote> remotes;
    for (const auto& favorite : favoriteRemotes) {
        Remote remote = getFavoriteRemoteToRemote(manufacturer, product, favorite);
        if (remote != remoteRepository.getEmptyRemote()) {
            remotes.push_back(remote);
        }
    }
    return remotes;
}

std::vector<FavoriteRemote> RemoteService::getDefaultFavoriteRemotes() {
    return remoteRepository.getDefaultFavoriteRemotes();
}

std::vector<FavoriteRemote> RemoteService::getUserFavoriteRemotes() {
    return remoteRepository.getUserFavoriteRemotes();
}


std::vector<FavoriteRemote> RemoteService::getAllFavoriteRemotes() {
    return remoteRepository.getAllFavoriteRemotes();
}

std::vector<FavoriteRemote> RemoteService::addFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& remoteName, const std::string& favoriteName) {
    return remoteRepository.saveUserFavoriteRemote(manufacturerName, productName, remoteName, favoriteName);
}

std::vector<FavoriteRemote> RemoteService::removeFavoriteRemote(const std::string& favoriteName, const std::string& productName) {
    return remoteRepository.deleteUserFavoriteRemote(favoriteName, productName);
}

}
