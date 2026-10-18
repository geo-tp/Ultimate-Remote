#pragma once
#include <Models/FavoriteRemote.h>
#include <string>
#include <vector>
namespace interfaces {
// Persistence only. Business rules (favorite limits and bundled defaults)
// remain in RemoteRepository and are shared by ESP32 and Linux.
class IFavoritesStorage {
public:
    virtual ~IFavoritesStorage() = default;
    virtual std::vector<models::FavoriteRemote> load() = 0;
    virtual bool append(const models::FavoriteRemote& favorite) = 0;
    virtual bool remove(const std::string& name, const std::string& product) = 0;
};
}
