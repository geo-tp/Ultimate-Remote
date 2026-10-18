#ifndef REMOTE_REPOSITORY_H
#define REMOTE_REPOSITORY_H

#include <vector>
#include <string>
#include <cstring>
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
#include <Interfaces/IFavoritesStorage.h>


// Maximum user favorites is enforced by RemoteRepository on every platform.

using namespace models;
using namespace contexts;
using namespace data;

namespace repositories {

    class RemoteRepository {
    public:
        explicit RemoteRepository(interfaces::IFavoritesStorage& storage);
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
        interfaces::IFavoritesStorage& storage;
        GlobalContext& context = GlobalContext::getInstance();

        // Ajout des variables constantes en tant que membres de classe
        static const Remote emptyRemote;
        static const RemoteCommand emptyRemoteCommand;
        static const FileRemote emptyFileRemote;
        static const FileRemoteCommand emptyFileRemoteCommand;
    };
}

#endif
