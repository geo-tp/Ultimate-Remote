
#ifndef MANUFACTURER_REPOSITORY_H
#define MANUFACTURER_REPOSITORY_H

#include <vector>
#include <string>
#include <cstring>
#include <Platform/Compatibility/PgmspaceCompat.h>
#include <algorithm>
#include <cctype>
#include <stddef.h>
#include <Models/Manufacturer.h>
#include <Contexts/GlobalContext.h>
#include <Utils/StringUtils.h>


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
            static size_t getEmbeddedRemoteCount();
            static size_t getEmbeddedManufacturerCount();

        private:
            static const std::vector<std::string> popularManufacturerNames;
            static const Manufacturer emptyManufacturer;
    };
}


#endif
