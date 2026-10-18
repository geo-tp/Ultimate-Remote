#ifdef ARDUINO
#include <Services/SdService.h>
#include <SD.h>
#include <SPI.h>
#include <algorithm>

namespace services {
namespace { SPIClass sdCardSPI; }
bool SdService::begin() {
    sdCardSPI.begin(globalContext.getSdCardCLKPin(), globalContext.getSdCardMISOPin(),
                    globalContext.getSdCardMOSIPin(), globalContext.getSdCardCSPin());
    delay(10);
    sdCardMounted = SD.begin(globalContext.getSdCardCSPin(), sdCardSPI);
    return sdCardMounted;
}
void SdService::close() {
    SD.end();
    sdCardMounted = false;
}
bool SdService::isFile(std::string filePath) {
    File file = SD.open(filePath.c_str());
    const bool result = file && !file.isDirectory();
    if (file) file.close();
    return result;
}
std::vector<std::string> SdService::listElements(std::string dirPath, size_t limit) {
    std::vector<std::string> files, folders;
    if (!sdCardMounted) return files;
    if (!limit) limit = globalContext.getFileCountLimit();
    File dir = SD.open(dirPath.c_str());
    if (!dir || !dir.isDirectory()) {
        if (dir) dir.close();
        return files;
    }
    size_t count = 0;
    while (true) {
        bool isDirectory = false;
        String entryPath = dir.getNextFileName(&isDirectory);
        if (entryPath.length() == 0) break;
        std::string name(entryPath.c_str());
        const auto slash = name.find_last_of('/');
        if (slash != std::string::npos) name.erase(0, slash + 1);
        if (!name.empty() && name[0] != '.') {
            if (isDirectory) folders.push_back(name);
            else files.push_back(name);
            if (++count > limit) break;
        }
    }
    dir.close();
    std::sort(files.begin(), files.end());
    std::sort(folders.begin(), folders.end());
    folders.insert(folders.end(), files.begin(), files.end());
    return folders;
}
std::string SdService::readFile(const char* filePath) {
    std::string result;
    if (!sdCardMounted) return result;
    File file = SD.open(filePath);
    if (file) {
        while (file.available()) result += static_cast<char>(file.read());
        file.close();
    }
    return result;
}
bool SdService::writeFile(const char* filePath, const std::string& data) {
    if (!sdCardMounted) return false;
    File file = SD.open(filePath, FILE_WRITE);
    if (!file) return false;
    const size_t written = file.write(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    file.close();
    return written == data.size();
}
bool SdService::appendToFile(const char* filePath, const std::string& data) {
    if (!sdCardMounted) return false;
    File file = SD.open(filePath, FILE_APPEND);
    if (!file) return false;
    const size_t written = file.write(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    file.close();
    return written == data.size();
}
}
#endif
