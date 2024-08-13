#ifndef SD_SERVICE_H
#define SD_SERVICE_H

#include <SD.h>
#include <SPI.h>
#include <vector>
#include <string>
#include <Contexts/GlobalContext.h>

using namespace contexts;

namespace services {


class SdService {
public:
    // Constructeur pour initialiser les pins SPI
    SdService();
    // Méthodes pour gérer la carte SD
    bool begin();           // Monter la carte SD
    void close();          // Démonter la carte SD
    bool isFile(std::string filePath);
    bool getSdState();

    // Méthodes pour gérer les fichiers et dossiers
    std::vector<std::string> listElements(std::string dirPath, size_t limit=256);
    std::string readFile(const char* filePath);                // Lire un fichier
    bool writeFile(const char* filePath, const std::string& data); // Écrire dans un fichier
    bool appendToFile(const char* filePath, const std::string& data); // Ajouter des données à un fichier

private:
    GlobalContext& context = GlobalContext::getInstance();
    SPIClass sdCardSPI;          // Instance SPI pour la carte SD
    bool sdCardMounted = false;  // État de montage de la carte SD
};

}
#endif // SD_SERVICE_H
