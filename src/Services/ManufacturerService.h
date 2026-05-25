#ifndef MANUFACTURER_SERVICE_H
#define MANUFACTURER_SERVICE_H

#include <vector>
#include <string>
#include <Models/Manufacturer.h>
#include <Repositories/ManufacturerRepository.h>
#include <Contexts/GlobalContext.h>

namespace services {

    class ManufacturerService {
    public:
        ManufacturerService(repositories::ManufacturerRepository& repository);
        ~ManufacturerService();

        std::vector<models::Manufacturer> getManufacturersBySubstring(const char* substring);
        std::vector<std::string> getManufacturerNamesBySubstring(const std::vector<std::string>& manufacturerNames, const char* substring);
        models::Manufacturer getManufacturer(const std::string& name);
        models::Manufacturer getEmptyManufacturer();
        std::vector<std::string> getAllManufacturerNames();
        std::vector<std::string> getPopularManufacturerNames();

    private:
        contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
        repositories::ManufacturerRepository& manufacturerRepository;
    };
}

#endif // MANUFACTURER_SERVICE_H
