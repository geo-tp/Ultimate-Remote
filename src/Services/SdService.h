#ifndef SD_SERVICE_H
#define SD_SERVICE_H
#include <vector>
#include <string>
#include <Contexts/GlobalContext.h>

namespace services {
// Cross-platform storage API. Arduino and Linux implement these methods in
// separate translation units rather than interleaving preprocessor branches.
class SdService {
public:
    SdService();
    bool begin();
    void close();
    bool isFile(std::string filePath);
    bool getSdState();
    std::vector<std::string> listElements(std::string dirPath, size_t limit = 0);
    std::string readFile(const char* filePath);
    bool writeFile(const char* filePath, const std::string& data);
    bool appendToFile(const char* filePath, const std::string& data);
private:
    contexts::GlobalContext& globalContext = contexts::GlobalContext::getInstance();
    bool sdCardMounted = false;
    std::string rootPath = ".";
};
}
#endif
