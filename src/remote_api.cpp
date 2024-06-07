/*

############################################################################################################################################
#                                                                                                                                          #
#                                                                                                                                          #
#                   An API that retrieves remote informations such as Manufacturer, Product, Remote, and RemoteCommand.                    #           
#                                                                                                                                          #
#                                                                                                                                          #
############################################################################################################################################

*/


#include "remote_api.h"

RemoteApi::RemoteApi() {
}

RemoteApi::~RemoteApi() {
}

std::vector<Manufacturer> RemoteApi::getManufacturersBySubstring(const char *substring) {
    std::vector<Manufacturer> foundManufacturers;

    char lowerSubstring[20];
    strncpy(lowerSubstring, substring, sizeof(lowerSubstring));
    lowerSubstring[sizeof(lowerSubstring) - 1] = '\0';
    for (size_t i = 0; i < strlen(lowerSubstring); ++i) {
        lowerSubstring[i] = tolower(lowerSubstring[i]);
    }

    for (size_t i = 0; i < NUM_MANUFACTURERS; ++i) {
        Manufacturer manufacturer;
        memcpy_P(&manufacturer, &manufacturers[i], sizeof(Manufacturer));

        char lowerManufacturerName[24];
        strncpy(lowerManufacturerName, manufacturer.name, sizeof(lowerManufacturerName));
        lowerManufacturerName[sizeof(lowerManufacturerName) - 1] = '\0';
        for (size_t j = 0; j < strlen(lowerManufacturerName); ++j) {
            lowerManufacturerName[j] = tolower(lowerManufacturerName[j]);
        }

        if (strstr(lowerManufacturerName, lowerSubstring) != NULL) {
            foundManufacturers.push_back(manufacturer);
        }
    }

    return foundManufacturers;
}

std::vector<std::string> RemoteApi::getManufacturerNamesBySubstring(const std::vector<std::string>& manufacturerNames, const char* substring) {
    std::vector<std::string> foundNames;

    std::string lowerSubstring(substring);
    std::transform(lowerSubstring.begin(), lowerSubstring.end(), lowerSubstring.begin(), ::tolower);

    for (const auto& name : manufacturerNames) {
        std::string lowerName = name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find(lowerSubstring) != std::string::npos) {
            foundNames.push_back(name);
        }
    }

    return foundNames;
}

std::vector<Product> RemoteApi::getProductBySubstring(const char* substring, const std::vector<Product>& products) {
    std::vector<Product> foundProducts;

    char lowerSubstring[30];
    strncpy(lowerSubstring, substring, sizeof(lowerSubstring));
    lowerSubstring[sizeof(lowerSubstring) - 1] = '\0';
    for (size_t i = 0; i < strlen(lowerSubstring); ++i) {
        lowerSubstring[i] = tolower(lowerSubstring[i]);
    }

    for (const auto& product : products) {
        char lowerProductName[30];
        strncpy(lowerProductName, product.name, sizeof(lowerProductName));
        lowerProductName[sizeof(lowerProductName) - 1] = '\0';
        for (size_t j = 0; j < strlen(lowerProductName); ++j) {
            lowerProductName[j] = tolower(lowerProductName[j]);
        }

        if (strstr(lowerProductName, lowerSubstring) != nullptr) {
            foundProducts.push_back(product);
        }
    }

    return foundProducts;
}

std::vector<Product> RemoteApi::getManufacturerProducts(const Manufacturer& manufacturer) {
    std::vector<Product> products;
    for (size_t i = 0; i < manufacturer.productCount; ++i) {
        Product product;
        memcpy_P(&product, &manufacturer.products[i], sizeof(Product));
        products.push_back(product);
    }
    return products;
}

std::vector<Remote> RemoteApi::getProductRemotes(const Product& product) {
    std::vector<Remote> remotes;
    for (size_t i = 0; i < product.remoteCount; ++i) {
        Remote remote;
        memcpy_P(&remote, &product.remotes[i], sizeof(Remote));
        remotes.push_back(remote);
    }
    return remotes;
}

std::vector<RemoteCommand> RemoteApi::getRemoteCommands(const Remote& remote) {
    std::vector<RemoteCommand> commands;
    for (size_t i = 0; i < remote.commandCount; ++i) {
        RemoteCommand command;
        memcpy_P(&command, &remote.commands[i], sizeof(RemoteCommand));
        commands.push_back(command);
    }
    return commands;
}

Manufacturer RemoteApi::getManufacturerByName(const std::string& name) {
    Manufacturer manufacturer = getEmptyManufacturer();

    if (name.empty()) {
        return manufacturer;
    }

    std::string upperName = name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), toUpperCase);


    for (size_t i = 0; i < NUM_MANUFACTURERS; ++i) {
        Manufacturer tempManufacturer;
        memcpy_P(&tempManufacturer, &manufacturers[i], sizeof(Manufacturer));

        std::string upperManufacturerName = tempManufacturer.name;
        std::transform(upperManufacturerName.begin(), upperManufacturerName.end(), upperManufacturerName.begin(), toUpperCase);

        if (upperName == upperManufacturerName) {
            return tempManufacturer;
        }
    }

    return manufacturer;
}

std::vector<std::string> RemoteApi::getPopularManufacturerNames() {
    return popularManufacturerNames;
}

std::vector<std::string> RemoteApi::getAllManufacturerNames() {
    std::vector<std::string> manufacturerNames;

    for (size_t i = 0; i < NUM_MANUFACTURERS; ++i) {
        Manufacturer manufacturer;
        memcpy_P(&manufacturer, &manufacturers[i], sizeof(Manufacturer));
        manufacturerNames.push_back(std::string(manufacturer.name));
    }

    std::sort(manufacturerNames.begin(), manufacturerNames.end());
    return manufacturerNames;
}

Product RemoteApi::getProductByName(const Manufacturer& manufacturer, const std::string& productName) {
    for (size_t i = 0; i < manufacturer.productCount; ++i) {
        Product product;
        memcpy_P(&product, &manufacturer.products[i], sizeof(Product));
        if (strcmp(product.name, productName.c_str()) == 0) {
            return product;
        }
    }
    return getEmptyProduct();
}

Remote RemoteApi::getRemoteByName(const Product& product, const std::string& remoteFileName) {
    for (size_t i = 0; i < product.remoteCount; ++i) {
        Remote remote;
        memcpy_P(&remote, &product.remotes[i], sizeof(Remote));
        if (strcmp(remote.fileName, remoteFileName.c_str()) == 0) {
            return remote;
        }
    }
    return getEmptyRemote();
}

Remote RemoteApi::getFavoriteRemoteToRemote(const FavoriteRemote& favoriteRemote) {
    // FavoriteRemote Struct to Remote Struct

    Manufacturer manufacturer = getManufacturerByName(favoriteRemote.manufacturerName);
    if (manufacturer.name[0] != '\0') {
        Product product = getProductByName(manufacturer, favoriteRemote.productName);
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
    return getEmptyRemote();
}

std::vector<Remote> RemoteApi::getFavoriteRemotesToRemotes(std::vector<FavoriteRemote> favoriteRemotes) {
    std::vector<Remote> remotes;
    for (const auto& favorite : favoriteRemotes) {
        Remote remote = getFavoriteRemoteToRemote(favorite);
        if (remote != getEmptyRemote()) {
            remotes.push_back(remote);
        }
    }
    return remotes;
}

std::vector<FavoriteRemote> RemoteApi::getUserFavoriteRemotes() {

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

std::vector<FavoriteRemote> RemoteApi::getDefaultFavoriteRemotes() {
    return defaultFavoriteRemotes;
}

std::vector<FavoriteRemote> RemoteApi::getAllFavoriteRemotes() {
    std::vector<FavoriteRemote> userFavorites = getUserFavoriteRemotes();
    std::vector<FavoriteRemote> defaultFavorites = getDefaultFavoriteRemotes();

    // Combiner les deux
    std::vector<FavoriteRemote> allFavorites = userFavorites;
    // Ajouter en premier les remotes users et les remotes par défaut à la fin
    allFavorites.insert(allFavorites.end(), defaultFavorites.begin(), defaultFavorites.end());

    return allFavorites;
}

Product RemoteApi::getFavoriteRemotesProduct() {
    static Remote remoteArray[MAX_FAVORITE_REMOTE_SAVED];
    Product favoriteProduct;
    favoriteProduct.remotes = remoteArray;
    favoriteProduct.remoteCount = 0;

    std::vector<FavoriteRemote> favoriteRemotes = getAllFavoriteRemotes();
    size_t count = 0;

    for (const auto& favoriteRemote : favoriteRemotes) {
        if (count >= MAX_FAVORITE_REMOTE_SAVED) {
            break;
        }
        
        Remote remote = getFavoriteRemoteToRemote(favoriteRemote);
        if (remote.fileName[0] != '\0') {
            // Copier les infos de la remote
            strncpy(remoteArray[count].fileName, remote.fileName, sizeof(remoteArray[count].fileName) - 1);
            remoteArray[count].fileName[sizeof(remoteArray[count].fileName) - 1] = '\0';
            remoteArray[count].commands = remote.commands;
            remoteArray[count].commandCount = remote.commandCount; 
            count++;
        }
    }

    // Copier le nombre de remotes dans le produit
    favoriteProduct.remoteCount = count;

    // Copier le nom du produit
    strncpy(favoriteProduct.name, "Favorites", sizeof(favoriteProduct.name) - 1);
    favoriteProduct.name[sizeof(favoriteProduct.name) - 1] = '\0';

    return favoriteProduct;
}


std::vector<FavoriteRemote> RemoteApi::saveUserFavoriteRemote(const std::string& manufacturerName, const std::string& productName, const std::string& fileName, const std::string& favoriteName) {
    
    // Init NVS memory with namespace
    preferences.begin(NVS_FAVORITE_NAMESPACE, false);

    // Récupérer le nombre actuel de remotes favorites
    size_t count = preferences.getUInt(NVS_FAVORITE_COUNT, 0);

    // We can't save more than MAX_FAVORITE_REMOTE_SAVED / 2 for user
    if (count >= MAX_FAVORITE_REMOTE_SAVED / 2 || manufacturerName.size() == 0) {
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

std::vector<FavoriteRemote> RemoteApi::deleteUserFavoriteRemote(const std::string& favoriteName, const std::string& productName) {
    

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

// Fonctions pour obtenir les objets vides

Manufacturer RemoteApi::getEmptyManufacturer() {
    return EMPTY_MANUFACTURER;
}

Product RemoteApi::getEmptyProduct() {
    return EMPTY_PRODUCT;
}

Remote RemoteApi::getEmptyRemote() {
    return EMPTY_REMOTE;
}

RemoteCommand RemoteApi::getEmptyRemoteCommand() {
    return EMPTY_REMOTE_COMMAND;
}

const char* RemoteApi::getProtocolString(ProtocolEnum protocol) {
    switch (protocol) {
        case _48_NEC1: return "nec1";
        case _48_NEC2: return "nec2";
        case AIWA: return "aiwa";
        case AKAI: return "akai";
        case _APPLE: return "apple";
        case BARCO: return "barco";
        case BLAUPUNKT: return "blaupunkt";
        case BOSE: return "bose";
        case CANALSAT: return "canalsat";
        case _DENON: return "denon";
        case DENON_K: return "Denon-K";
        case DENON_1_: return "denon";
        case DENON_2_: return "denon";
        case DGTEC: return "Dgtec";
        case DIRECTV: return "directv";
        case DISH_NETWORK: return "DishPlayer_Network";
        case DISHPLAYER: return "DishPlayer_Network";
        case EMERSON: return "emerson";
        case F12: return "f12";
        case FUJITSU: return "fujitsu";
        case G_I_4DTV: return "GI4dtv";
        case G_I_CABLE: return "GI_cable";
        case GXB: return "gxb";
        case GRUNDIG16: return "grundig16";
        case GRUNDIG16_30: return "grundig16_30";
        case _JVC: return "jvc";
        case JVC_48: return "jvc";
        case JVC_2_: return "jvc_two_frames";
        case JERROLD: return "Jerrold";
        case KATHREIN: return "kathrein";
        case KONKA: return "konka";
        case LOGITECH: return "logitech";
        case LUMAGEN: return "lumagen";
        case MCE: return "mce";
        case MITSUBISHI: return "Mitsubishi";
        case _NEC: return "nec";
        case NEC1: return "nec1";
        case NEC1_F16: return "nec1_f16";
        case NEC1_RNC: return "nec1_rnc";
        case NEC1_Y1: return "nec1_y1";
        case NEC1_Y2: return "nec1_y2";
        case NEC1_Y3: return "nec1_y3";
        case _NEC2: return "nec2";
        case NEC2_F16: return "nec2_f16";
        case NECX1: return "NECx1";
        case NECX2: return "NECx2";
        case NRC16: return "nrc16";
        case NRC17: return "nrc17";
        case NOKIA: return "Nokia32";
        case NOKIA12: return "Nokia32single";
        case NOKIA32: return "Nokia32";
        case PACEMSS: return "pacemss";
        case _PANASONIC: return "panasonic";
        case PANASONIC2: return "panasonic2";
        case PANASONIC_OLD: return "panasonic_old";
        case PIONEER: return "pioneer";
        case PROTON: return "Proton";
        case _RC5: return "rc5";
        case RC5_7F: return "rc5odd";
        case _RC6: return "rc6";
        case RC6_6_20: return "rc6-6-20";
        case RCA: return "rca";
        case RCA_OLD_: return "rca";
        case RCA_38: return "rca";
        case RCA_38_OLD_: return "rca";
        case RECS80: return "recs80_45";
        case REPLAY: return "replay";
        case SAMSUNG20: return "Samsung20";
        case SAMSUNG36: return "Samsung36";
        case SCATL_6: return "scatl_6";
        case SEJIN_1_38: return "sejin_1_38";
        case SEJIN_1_56: return "sejin_1_56";
        case _SHARP: return "sharp";
        case SHARPDVD: return "sharp";
        case SHARP_1_: return "sharp";
        case SHARP_2_: return "sharp";
        case SONY12: return "sony12";
        case SONY15: return "sony15";
        case SONY20: return "sony20";
        case STREAMZAP: return "streamzap";
        case TEAC_K: return "Teac-K";
        case THOMSON: return "Thomson";
        case THOMSON7: return "Thomson";
        case TIVO_UNIT_0: return "Tivo-Nec1";
        case TIVO_UNIT_1: return "Tivo-Nec1";
        case TIVO_UNIT_2: return "Tivo-Nec1";
        case TIVO_UNIT_3: return "Tivo-Nec1";
        case TIVO_UNIT_4: return "Tivo-Nec1";
        case TIVO_UNIT_5: return "Tivo-Nec1";
        case TIVO_UNIT_6: return "Tivo-Nec1";
        case TIVO_UNIT_7: return "Tivo-Nec1";
        case TIVO_UNIT_8: return "Tivo-Nec1";
        case TIVO_UNIT_9: return "Tivo-Nec1";
        case VELLEMAN: return "velleman";
        case XMP: return "XMP";
        case XMP_1: return "XMP";
        case XMP_2: return "XMP";
        case ZAPTOR_56: return "zaptor_56";
        case ZENITH: return "zenith";
        case DISH_NETWO: return "DishPlayer_Network";
        case PANASONIC_: return "panasonic";
        case TIVO_UNIT: return "Tivo-Nec1";
        case RCA_38_OLD: return "rca";
        default: return "nec2"; // // Most common, NEC2 for reliabity which continuously sends the full command frame
    }
}