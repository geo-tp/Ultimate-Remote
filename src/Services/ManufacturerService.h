#ifndef MANUFACTURER_SERVICE_H
#define MANUFACTURER_SERVICE_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <pgmspace.h>
#include <Models/Manufacturer.h>
#include <Repositories/ManufacturerRepository.h>

using namespace models;
using namespace repositories;

namespace services {

    class ManufacturerService {
    public:
        ManufacturerService();
        ~ManufacturerService();

        std::vector<Manufacturer> getManufacturersBySubstring(const char *substring);
        std::vector<std::string> getManufacturerNamesBySubstring(const std::vector<std::string>& manufacturerNames, const char* substring);
        Manufacturer getManufacturer(const std::string& name);
        Manufacturer getEmptyManufacturer();
        std::vector<std::string> getAllManufacturerNames();
        std::vector<std::string> getPopularManufacturerNames();
    private:
       ManufacturerRepository manufacturerRepository;
    };
}

#endif // MANUFACTURER_SERVICE_H
