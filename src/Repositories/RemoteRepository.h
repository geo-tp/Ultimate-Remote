#ifndef REMOTE_REPOSITORY_H
#define REMOTE_REPOSITORY_H

#include <vector>
#include <string>
#include <cstring>
#include <Preferences.h>
#include <Models/FavoriteRemote.h>
#include <Models/Remote.h>
#include <Models/FileRemote.h>
#include <Models/Product.h>
#include <Models/Manufacturer.h>
#include <Models/RemoteCommand.h>
#include <Models/FileRemoteCommand.h>
#include <Enums/ProtocolEnum.h>
#include <Data/DefaultFavoriteRemotes.h>
#include <Contexts/GlobalContext.h>

// We use NVS to store favorite remotes, we can't save an infinity of remotes
// 40 for user favorites, 40 for default favorites
#define NVS_FAVORITE_NAMESPACE "favoriteRemotes"
#define NVS_FAVORITE_COUNT "favoriteCount"

namespace repositories {

    class RemoteRepository {
    public:
        RemoteRepository();
        ~RemoteRepository();

        std::vector<models::FavoriteRemote> getUserFavoriteRemotes();
        static std::vector<models::FavoriteRemote> getDefaultFavoriteRemotes();
        std::vector<models::FavoriteRemote> getAllFavoriteRemotes();

        std::vector<models::FavoriteRemote> saveUserFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& fileName, const std::string& favoriteName);
        std::vector<models::FavoriteRemote> deleteUserFavoriteRemote(const std::string& favoriteName, const std::string& productName);

        static models::Remote getEmptyRemote();
        static models::RemoteCommand getEmptyRemoteCommand();
        static models::FileRemote getEmptyFileRemote();
        static models::FileRemoteCommand getEmptyFileRemoteCommand();
    private:
        Preferences preferences; // NVS, used to get/save/delete user favorite remotes
        contexts::GlobalContext& context = contexts::GlobalContext::getInstance();

        // Ajout des variables constantes en tant que membres de classe
        static const models::Remote emptyRemote;
        static const models::RemoteCommand emptyRemoteCommand;
        static const models::FileRemote emptyFileRemote;
        static const models::FileRemoteCommand emptyFileRemoteCommand;
    };
}

#endif
