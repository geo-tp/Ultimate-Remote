#pragma once

#include <cctype>
#include <string>

namespace utils {

// Expand abbreviated labels supplied with the built-in universal presets.
// This only changes the displayed label; persistent favorite IDs stay intact.
inline std::string universalFavoriteLabel(std::string name) {
    for (size_t i = 0; i < name.size(); ++i) {
        if (i != 0 && name[i - 1] != ' ' && name[i - 1] != '-') continue;
        const bool uni = name.compare(i, 3, "Uni") == 0 &&
            (i + 3 == name.size() || name[i + 3] == ' ' ||
             std::isdigit(static_cast<unsigned char>(name[i + 3])));
        const bool un = !uni && name.compare(i, 2, "Un") == 0 &&
            i + 2 < name.size() && std::isdigit(static_cast<unsigned char>(name[i + 2]));
        if (uni || un) {
            name.replace(i, uni ? 3 : 2, "Universal");
            i += std::string("Universal").size() - 1;
        }
    }
    return name;
}

} // namespace utils
