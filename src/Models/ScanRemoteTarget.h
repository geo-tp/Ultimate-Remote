#pragma once

#include <Models/Remote.h>
#include <string>

namespace models {

struct ScanRemoteTarget {
    std::string productName;
    Remote remote;
};

}
