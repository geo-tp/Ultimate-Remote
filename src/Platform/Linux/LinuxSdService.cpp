#ifndef ARDUINO
#include <Services/SdService.h>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace services {
bool SdService::begin() {
    const char* envRoot = std::getenv("ULTIMATE_REMOTE_SD_ROOT");
    rootPath = envRoot ? envRoot : ".";
    sdCardMounted = std::filesystem::exists(rootPath);
    return sdCardMounted;
}
void SdService::close() { sdCardMounted = false; }
bool SdService::isFile(std::string filePath) {
    return std::filesystem::is_regular_file(
        std::filesystem::path(rootPath) / std::filesystem::path(filePath).relative_path());
}
std::vector<std::string> SdService::listElements(std::string dirPath, size_t limit) {
    if (!sdCardMounted) return {};
    if (!limit) limit = globalContext.getFileCountLimit();
    std::vector<std::string> files, folders;
    const auto path = std::filesystem::path(rootPath) / std::filesystem::path(dirPath).relative_path();
    if (!std::filesystem::is_directory(path)) return files;
    size_t count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const std::string name = entry.path().filename().string();
        if (!name.empty() && name[0] != '.') {
            if (entry.is_directory()) folders.push_back(name);
            else files.push_back(name);
            if (++count > limit) break;
        }
    }
    std::sort(files.begin(), files.end());
    std::sort(folders.begin(), folders.end());
    folders.insert(folders.end(), files.begin(), files.end());
    return folders;
}
std::string SdService::readFile(const char* filePath) {
    if (!sdCardMounted) return {};
    std::ifstream file(std::filesystem::path(rootPath) / std::filesystem::path(filePath).relative_path(), std::ios::binary);
    std::ostringstream out;
    out << file.rdbuf();
    return out.str();
}
bool SdService::writeFile(const char* filePath, const std::string& data) {
    if (!sdCardMounted) return false;
    std::ofstream file(std::filesystem::path(rootPath) / std::filesystem::path(filePath).relative_path(),
                       std::ios::binary | std::ios::trunc);
    file << data;
    return file.good();
}
bool SdService::appendToFile(const char* filePath, const std::string& data) {
    if (!sdCardMounted) return false;
    std::ofstream file(std::filesystem::path(rootPath) / std::filesystem::path(filePath).relative_path(),
                       std::ios::binary | std::ios::app);
    file << data;
    return file.good();
}
}
#endif
