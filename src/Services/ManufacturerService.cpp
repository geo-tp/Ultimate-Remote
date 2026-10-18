#include "ManufacturerService.h"
#include <algorithm>
#include <cctype>

namespace services {


ManufacturerService::ManufacturerService(ManufacturerRepository& repository)
    : manufacturerRepository(repository) {}
    
ManufacturerService::~ManufacturerService() {
}

std::vector<Manufacturer> ManufacturerService::getManufacturersBySubstring(const char *substring) {
    return manufacturerRepository.getManufacturersBySubstring(substring);
}

std::vector<std::string> ManufacturerService::getManufacturerNamesBySubstring(const std::vector<std::string>& manufacturerNames, const char* substring) {
    std::vector<std::string> foundNames;

    std::string lowerSubstring(substring);
    std::transform(lowerSubstring.begin(), lowerSubstring.end(), lowerSubstring.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (const auto& name : manufacturerNames) {
        std::string lowerName = name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (lowerName.find(lowerSubstring) != std::string::npos) {
            foundNames.push_back(name);
        }
    }

    return foundNames;
}

Manufacturer ManufacturerService::getManufacturer(const std::string& name) {
    return manufacturerRepository.getManufacturerByName(name);
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