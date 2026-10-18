#pragma once
#include <Models/InfoPage.h>

#include <cstddef>
#include <string>
#include <vector>

namespace data {


// Shared by the application and the framebuffer preview. Each page fits three
// body lines at the device's native resolution; database counts come from data.
inline std::vector<models::InfoPage> getAppInfoPages(size_t remoteCount, size_t manufacturerCount) {
    return {
        {std::to_string(remoteCount) + " embedded remotes", {
            "Built in: works without Internet.",
            std::to_string(manufacturerCount) + " manufacturers in Remotes.",
            "Source: probonopd/irdb."
        }},
#ifndef ARDUINO
        {"Thousands more online", {
            "Online IR opens Flipper-IRDB.",
            "Browse category, brand and model.",
            "Source: Lucaslhm/Flipper-IRDB."
        }},
        {"Download once", {
            "Internet needed for new remotes.",
            "Downloaded remotes work offline.",
            "Refresh online catalog for updates."
        }},
#endif
        {"Find & save", {
            "Scan tests a brand's remotes.",
            "SPACE saves a working remote.",
            "Open Favorites for quick access."
        }},
    };
}

} // namespace data
