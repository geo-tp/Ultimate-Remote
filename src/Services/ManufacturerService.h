#ifndef MANUFACTURER_SERVICE_H
#define MANUFACTURER_SERVICE_H

#include <vector>
#include <string>
#include <Models/Manufacturer.h>
#include <Repositories/ManufacturerRepository.h>
#include <Contexts/GlobalContext.h>

using namespace models;
using namespace repositories;
using namespace contexts;

namespace services {

    class ManufacturerService {
    public:
        ManufacturerService(ManufacturerRepository& repository);
        ~ManufacturerService();

        std::vector<Manufacturer> getManufacturersBySubstring(const char* substring);
        std::vector<std::string> getManufacturerNamesBySubstring(const std::vector<std::string>& manufacturerNames, const char* substring);
        Manufacturer getManufacturer(const std::string& name);
        Manufacturer getEmptyManufacturer();
        std::vector<std::string> getAllManufacturerNames();
        std::vector<std::string> getPopularManufacturerNames();

    private:
        GlobalContext& globalContext = GlobalContext::getInstance();
        ManufacturerRepository& manufacturerRepository;
    };
}

#endif // MANUFACTURER_SERVICE_H
