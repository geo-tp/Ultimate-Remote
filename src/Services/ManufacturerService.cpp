#include "ManufacturerService.h"

namespace services {


ManufacturerService::ManufacturerService() {
}

ManufacturerService::~ManufacturerService() {
}

std::vector<Manufacturer> ManufacturerService::getManufacturersBySubstring(const char *substring) {
    return manufacturerRepository.getManufacturersBySubstring(substring);
}

std::vector<std::string> ManufacturerService::getManufacturerNamesBySubstring(const std::vector<std::string>& manufacturerNames, const char* substring) {
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

Manufacturer ManufacturerService::getManufacturer(const std::string& name) {
    Manufacturer manufacturer = manufacturerRepository.getEmptyManufacturer();

    if (name.empty()) {
        return manufacturer;
    }

    std::string upperName = name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);


    for (size_t i = 0; i < context.getManufacturerCount(); ++i) {
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

Manufacturer ManufacturerService::getEmptyManufacturer() {
    return manufacturerRepository.getEmptyManufacturer();
}

std::vector<std::string> ManufacturerService::getAllManufacturerNames() {
    return manufacturerRepository.getAllManufacturerNames();
}


std::vector<std::string> ManufacturerService::getPopularManufacturerNames() {
    return manufacturerRepository.getPopularManufacturerNames();
}


}