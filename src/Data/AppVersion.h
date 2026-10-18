#pragma once

// Shared by the application, CMake project and Debian package builder.
#define ULTIMATE_REMOTE_VERSION "1.3"

namespace data {
inline constexpr char AppVersion[] = ULTIMATE_REMOTE_VERSION;
}
