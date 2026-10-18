#pragma once
#include <Interfaces/IFavoritesStorage.h>
namespace platform::arduino {
class ArduinoFavoritesStorage final : public interfaces::IFavoritesStorage {
public:
    std::vector<models::FavoriteRemote> load() override;
    bool append(const models::FavoriteRemote& favorite) override;
    bool remove(const std::string& name, const std::string& product) override;
};
}
