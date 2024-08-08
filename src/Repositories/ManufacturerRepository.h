
#ifndef MANUFACTURER_REPOSITORY_H
#define MANUFACTURER_REPOSITORY_H

#include <vector>
#include <string>
#include <cstring>
#include <pgmspace.h>
#include <algorithm>
#include <cctype>
#include <stddef.h>
#include <Models/Manufacturer.h>
#include <Data/Database.h>
#include <Contexts/GlobalContext.h>

using namespace models;
using namespace contexts;

namespace repositories {

    class ManufacturerRepository {
        public:
            ManufacturerRepository();
            ~ManufacturerRepository();

            static std::vector<std::string> getPopularManufacturerNames();
            static std::vector<std::string> getAllManufacturerNames();
            static std::vector<Manufacturer> getManufacturersBySubstring(const char *substring);
            static Manufacturer getManufacturerByName(const std::string& name);
            static Manufacturer getEmptyManufacturer();

        private:
            static GlobalContext& context;
            static const std::vector<std::string> popularManufacturerNames;
            static const Manufacturer emptyManufacturer;
    };
}


#endif