/*

############################################################################################################################################
#                                                                                                                                          #
#                                                                                                                                          #
#                   An API that retrieves remote informations such as Manufacturer, Product, Remote, and RemoteCommands.                   #           
#                                                                                                                                          #
#                                                                                                                                          #
############################################################################################################################################

*/


#ifndef REMOTE_API_H
#define REMOTE_API_H

#include <vector>
#include <string>
#include <cstring>
#include <M5Cardputer.h>
#include <Preferences.h>
#include "remote_data.h"

// We use NVS to store favorite remotes, we can't save an infinity of remotes
// 40 for user favorites, 40 for default favorites
#define MAX_FAVORITE_REMOTE_SAVED 80
#define NVS_FAVORITE_NAMESPACE "favoriteRemotes"
#define NVS_FAVORITE_COUNT "favoriteCount"

class RemoteApi {
public:
    RemoteApi();
    ~RemoteApi();

    static std::vector<std::string> getPopularManufacturerNames();
    static std::vector<std::string> getAllManufacturerNames();

    static std::vector<Manufacturer> getManufacturersBySubstring(const char *substring);
    static std::vector<std::string> getManufacturerNamesBySubstring(const std::vector<std::string>& manufacturerNames, const char* substring);
    static std::vector<Product> getProductBySubstring(const char* substring, const std::vector<Product>& products);
    
    static std::vector<Product> getManufacturerProducts(const Manufacturer& manufacturer);
    static std::vector<Remote> getProductRemotes(const Product& product);
    static std::vector<RemoteCommand> getRemoteCommands(const Remote& remote);

    static Manufacturer getManufacturerByName(const std::string& name);
    static Product getProductByName(const Manufacturer& manufacturer, const std::string& productName);
    static Remote getRemoteByName(const Product& product, const std::string& remoteFileName);
    
    std::vector<FavoriteRemote> getUserFavoriteRemotes();
    static std::vector<FavoriteRemote> getDefaultFavoriteRemotes();
    std::vector<FavoriteRemote> getAllFavoriteRemotes();
    Product getFavoriteRemotesProduct();
    
    static Remote getFavoriteRemoteToRemote(const FavoriteRemote& favoriteRemote);
    static std::vector<Remote> getFavoriteRemotesToRemotes(std::vector<FavoriteRemote> favoriteRemotes);

    std::vector<FavoriteRemote> saveUserFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& fileName, const std::string& favoriteName);
    std::vector<FavoriteRemote> deleteUserFavoriteRemote(const std::string& favoriteName, const std::string& productName);

    // Fonctions pour obtenir les objets vides
    static Manufacturer getEmptyManufacturer();
    static Product getEmptyProduct();
    static Remote getEmptyRemote();
    static RemoteCommand getEmptyRemoteCommand();

    static const char* getProtocolString(ProtocolEnum protocol);
private:
    Preferences preferences; // NVS, used to get/save/delete user favorite remotes
};

#endif // REMOTE_API_H
