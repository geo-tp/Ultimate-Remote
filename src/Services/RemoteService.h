#ifndef REMOTE_SERVICE_H
#define REMOTE_SERVICE_H

#include <vector>
#include <string>
#include <Repositories/RemoteRepository.h>
#include <Repositories/ManufacturerRepository.h>
#include <Repositories/ProductRepository.h>

using namespace models;
using namespace repositories;

namespace services {

    class RemoteService {
    public:
        RemoteService();
        ~RemoteService();

        // Fetch a remote by its name from a given  product
        Remote getRemote(const Product& product, const std::string& remoteFileName);
        std::vector<Remote> getRemotes(const Product& product);

        Remote getEmptyRemote();
        RemoteCommand getEmptyRemoteCommand();
        FileRemote getEmptyFileRemote();
        FileRemoteCommand getEmptyFileRemoteCommand();

        std::vector<RemoteCommand> getRemoteCommands(const Remote& remote);

        std::vector<FavoriteRemote> getDefaultFavoriteRemotes();
        std::vector<FavoriteRemote> getUserFavoriteRemotes();
        std::vector<FavoriteRemote> getAllFavoriteRemotes();

        Remote getFavoriteRemoteToRemote(Manufacturer manufacturer, Product product, const FavoriteRemote& favoriteRemote);
        std::vector<Remote> getFavoriteRemotesToRemotes(Manufacturer manufacturer, Product product, std::vector<FavoriteRemote> favoriteRemotes);

        // Add a remote to favorites
        std::vector<FavoriteRemote> addFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& remoteName, const std::string& favoriteName);

        // Remove a remote from favorites
        std::vector<FavoriteRemote> removeFavoriteRemote(const std::string& favoriteName, const std::string& productName);

    private:
        RemoteRepository remoteRepository;
    };

} // namespace services

#endif // REMOTE_SERVICE_H
