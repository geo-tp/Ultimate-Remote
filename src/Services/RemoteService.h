#ifndef REMOTE_SERVICE_H
#define REMOTE_SERVICE_H

#include <vector>
#include <string>
#include <Repositories/RemoteRepository.h>
#include <Repositories/ManufacturerRepository.h>
#include <Repositories/ProductRepository.h>

namespace services {

    class RemoteService {
    public:
        RemoteService(repositories::RemoteRepository& repository);
        ~RemoteService();

        Remote getRemote(const models::Product& product, const std::string& remoteFileName);
        std::vector<models::Remote> getRemotes(const models::Product& product);

        models::Remote getEmptyRemote();
        models::RemoteCommand getEmptyRemoteCommand();
        models::FileRemote getEmptyFileRemote();
        models::FileRemoteCommand getEmptyFileRemoteCommand();

        std::vector<models::RemoteCommand> getRemoteCommands(const models::Remote& remote);

        std::vector<models::FavoriteRemote> getDefaultFavoriteRemotes();
        std::vector<models::FavoriteRemote> getUserFavoriteRemotes();
        std::vector<models::FavoriteRemote> getAllFavoriteRemotes();

        models::Remote getFavoriteRemoteToRemote(models::Manufacturer manufacturer, models::Product product, const models::FavoriteRemote& favoriteRemote);
        std::vector<models::Remote> getFavoriteRemotesToRemotes(models::Manufacturer manufacturer, models::Product product, std::vector<models::FavoriteRemote> favoriteRemotes);

        std::vector<models::FavoriteRemote> addFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& remoteName, const std::string& favoriteName);

        std::vector<models::FavoriteRemote> removeFavoriteRemote(const std::string& favoriteName, const std::string& productName);

    private:
        repositories::RemoteRepository remoteRepository;
    };

} // namespace services

#endif // REMOTE_SERVICE_H
