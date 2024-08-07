
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

#define NUM_MANUFACTURERS 634

using namespace models;

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
            static const std::vector<std::string> popularManufacturerNames;
            static const Manufacturer emptyManufacturer;
    };
}


#endif