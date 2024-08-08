#include "ManufacturerRepository.h"
#include <pgmspace.h>
#include <Utils/StringUtils.h>

namespace repositories {


ManufacturerRepository::ManufacturerRepository() {
}

ManufacturerRepository::~ManufacturerRepository() {
}

// Définition de la liste des fabricants populaires
const std::vector<std::string> ManufacturerRepository::popularManufacturerNames = {
    "Sony", "Samsung", "LG", "Panasonic", "Philips",
    "Sharp", "Toshiba", "Vizio", "JVC", "Pioneer",
    "Yamaha", "Denon", "Marantz", "Onkyo",
    "Harman Kardon", "Apple", "Canon", "Akai",
    "Fujitsu", "Kenwood", "Microsoft", "NEC", "Amazon",
    "Vizio", "Roku"
};

const Manufacturer ManufacturerRepository::emptyManufacturer = {"", nullptr, 0};

Manufacturer ManufacturerRepository::getEmptyManufacturer() {
    return emptyManufacturer;
}

std::vector<std::string> ManufacturerRepository::getPopularManufacturerNames() {
    return popularManufacturerNames;
}

std::vector<std::string> ManufacturerRepository::getAllManufacturerNames() {
    std::vector<std::string> manufacturerNames;
    GlobalContext& context = GlobalContext::getInstance();

    for (size_t i = 0; i < context.getManufacturerCount() ; ++i) {
        Manufacturer manufacturer;
        memcpy_P(&manufacturer, &manufacturers[i], sizeof(Manufacturer));
        manufacturerNames.push_back(std::string(manufacturer.name));
    }

    std::sort(manufacturerNames.begin(), manufacturerNames.end());
    return manufacturerNames;
}

std::vector<Manufacturer> ManufacturerRepository::getManufacturersBySubstring(const char *substring) {
    std::vector<Manufacturer> foundManufacturers;
    GlobalContext& context = GlobalContext::getInstance();

    char lowerSubstring[20];
    strncpy(lowerSubstring, substring, sizeof(lowerSubstring));
    lowerSubstring[sizeof(lowerSubstring) - 1] = '\0';
    for (size_t i = 0; i < strlen(lowerSubstring); ++i) {
        lowerSubstring[i] = tolower(lowerSubstring[i]);
    }

    for (size_t i = 0; i < context.getManufacturerCount() ; ++i) {
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

Manufacturer ManufacturerRepository::getManufacturerByName(const std::string& name) {
    Manufacturer manufacturer = getEmptyManufacturer();
    GlobalContext& context = GlobalContext::getInstance();

    if (name.empty()) {
        return manufacturer;
    }

    std::string upperName = name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);


    for (size_t i = 0; i < context.getManufacturerCount() ; ++i) {
        Manufacturer tempManufacturer;
        memcpy_P(&tempManufacturer, &manufacturers[i], sizeof(Manufacturer));

        std::string upperManufacturerName = tempManufacturer.name;
        std::transform(upperManufacturerName.begin(), upperManufacturerName.end(), upperManufacturerName.begin(), ::toupper);

        if (upperName == upperManufacturerName) {
            return tempManufacturer;
        }
    }

    return manufacturer;
}

}