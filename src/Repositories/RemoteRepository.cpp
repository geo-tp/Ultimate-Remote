#include "RemoteRepository.h"

namespace repositories {

const Remote RemoteRepository::emptyRemote = {"", nullptr, 0};
const FileRemote RemoteRepository::emptyFileRemote = {"", {}};

const RemoteCommand RemoteRepository::emptyRemoteCommand = {"", ProtocolEnum(0), -1, -1, -1};
const FileRemoteCommand RemoteRepository::emptyFileRemoteCommand = {"", ProtocolEnum(0), 0, 0, 0, 0, 0, 0.0};

RemoteRepository::RemoteRepository() {
}

RemoteRepository::~RemoteRepository() {
}

std::vector<FavoriteRemote> RemoteRepository::getUserFavoriteRemotes() {

    // Init mémoire avec le namespace
    preferences.begin(NVS_FAVORITE_NAMESPACE, true);

    std::vector<FavoriteRemote> favoriteRemotes;

    // Récupérer le nombre de remotes favorites
    size_t count = preferences.getUInt(NVS_FAVORITE_COUNT, 0);

    // Parser les chaines de char pour obtenir des struct FavoriteRemote
    for (size_t i = 0; i < count; ++i) {
        FavoriteRemote favoriteRemote;
        String key = "favorite" + String(i);
        String value = preferences.getString(key.c_str(), "");
        if (value.length() > 0) {
            sscanf(value.c_str(), "%23[^|]|%29[^|]|%13[^|]|%13[^|]", 
                favoriteRemote.manufacturerName, 
                favoriteRemote.productName, 
                favoriteRemote.remoteFileName, 
                favoriteRemote.favoriteName);
            favoriteRemotes.push_back(favoriteRemote);
        }
    }

    // release
    preferences.end();

    // Inverser l'ordre du vecteur
    std::reverse(favoriteRemotes.begin(), favoriteRemotes.end());

    return favoriteRemotes;
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

std::vector<FavoriteRemote> RemoteRepository::saveUserFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& fileName, const std::string& favoriteName) {
    
    // Init NVS memory with namespace
    preferences.begin(NVS_FAVORITE_NAMESPACE, false);

    // Récupérer le nombre actuel de remotes favorites
    size_t count = preferences.getUInt(NVS_FAVORITE_COUNT, 0);

    // We can't save more than MAX_FAVORITE_REMOTE_SAVED / 2 for user
    if (count >= context.getMaxFavorites() / 2 || manufacturerName.size() == 0) {
        preferences.end();
        return getUserFavoriteRemotes();
    }

    // Formater les données
    char value[128];
    snprintf(value, sizeof(value), "%s|%s|%s|%s", 
             manufacturerName.c_str(), 
             productName.c_str(), 
             fileName.c_str(), 
             favoriteName.c_str());

    // Enregistrer la nouvelle remote
    String key = "favorite" + String(count);
    preferences.putString(key.c_str(), value);

    // Mettre à jour le compteur
    preferences.putUInt(NVS_FAVORITE_COUNT, count + 1);

    // release before call getUserFavoriteRemotes
    preferences.end();

    // Récupérer la liste mise a jour
    std::vector<FavoriteRemote> updatedFavorites = getUserFavoriteRemotes();

    return updatedFavorites;
}

std::vector<FavoriteRemote> RemoteRepository::deleteUserFavoriteRemote(const std::string& favoriteName, const std::string& productName) {
    

    // Init NVS with namespace
    preferences.begin(NVS_FAVORITE_NAMESPACE, false);

    size_t count = preferences.getUInt(NVS_FAVORITE_COUNT, 0);

    // release before call getUserFavoriteRemotes
    preferences.end();

    // Remotes favorites de l'utilisateur
    std::vector<FavoriteRemote> favoriteRemotes = getUserFavoriteRemotes();

    // Inverser l'ordre du vecteur
    std::reverse(favoriteRemotes.begin(), favoriteRemotes.end());

    // Trouver et supprimer la remote favorite
    auto it = std::remove_if(favoriteRemotes.begin(), favoriteRemotes.end(),
                             [&](const FavoriteRemote& remote) {
                                 return std::string(remote.favoriteName) == favoriteName &&
                                        std::string(remote.productName) == productName;
                             });

    if (it != favoriteRemotes.end()) {
        favoriteRemotes.erase(it, favoriteRemotes.end());
        
        // init NVS avec le namespace
        preferences.begin(NVS_FAVORITE_NAMESPACE, false);

        // Effacer toutes les prefs
        preferences.clear();

        // Rewrite remotes
        preferences.putUInt(NVS_FAVORITE_COUNT, favoriteRemotes.size());
        for (size_t i = 0; i < favoriteRemotes.size(); ++i) {
            char value[100];
            snprintf(value, sizeof(value), "%s|%s|%s|%s", 
                     favoriteRemotes[i].manufacturerName, 
                     favoriteRemotes[i].productName, 
                     favoriteRemotes[i].remoteFileName, 
                     favoriteRemotes[i].favoriteName);

            String key = "favorite" + String(i);
            preferences.putString(key.c_str(), value);

        }
        
        // release
        preferences.end();
    }

    // Liste a jour
    std::vector<FavoriteRemote> updatedFavorites = getUserFavoriteRemotes();


    return updatedFavorites;
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