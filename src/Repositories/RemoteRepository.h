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
#include <Data/ProtocolEnum.h>
#include <Data/DefaultFavoriteRemotes.h>
#include <Contexts/GlobalContext.h>

// We use NVS to store favorite remotes, we can't save an infinity of remotes
// 40 for user favorites, 40 for default favorites
#define NVS_FAVORITE_NAMESPACE "favoriteRemotes"
#define NVS_FAVORITE_COUNT "favoriteCount"

using namespace models;
using namespace contexts;

namespace repositories {

    class RemoteRepository {
    public:
        RemoteRepository();
        ~RemoteRepository();
        
        std::vector<FavoriteRemote> getUserFavoriteRemotes();
        static std::vector<FavoriteRemote> getDefaultFavoriteRemotes();
        std::vector<FavoriteRemote> getAllFavoriteRemotes();
        
        std::vector<FavoriteRemote> saveUserFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& fileName, const std::string& favoriteName);
        std::vector<FavoriteRemote> deleteUserFavoriteRemote(const std::string& favoriteName, const std::string& productName);

        static Remote getEmptyRemote();
        static RemoteCommand getEmptyRemoteCommand();
        static FileRemote getEmptyFileRemote();
        static FileRemoteCommand getEmptyFileRemoteCommand();
    private:
        Preferences preferences; // NVS, used to get/save/delete user favorite remotes
        GlobalContext& context = GlobalContext::getInstance();

        // Ajout des variables constantes en tant que membres de classe
        static const Remote emptyRemote;
        static const RemoteCommand emptyRemoteCommand;
        static const FileRemote emptyFileRemote;
        static const FileRemoteCommand emptyFileRemoteCommand;
    };
}

#endif
