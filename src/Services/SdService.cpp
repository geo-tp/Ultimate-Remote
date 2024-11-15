#include "SdService.h"

namespace services {

SdService::SdService(GlobalContext& globalContext)
    : globalContext(globalContext) {}

bool SdService::begin() {
sdCardSPI.begin(
    globalContext.getSdCardCLKPin(),
    globalContext.getSdCardMISOPin(),
    globalContext.getSdCardMOSIPin(),
    globalContext.getSdCardCSPin()
);
    delay(10);

    if (!SD.begin(globalContext.getSdCardCSPin(), sdCardSPI)) {
        sdCardMounted = false;
        return false;
    } else {
        sdCardMounted = true;
    }

    return sdCardMounted;
}

void SdService::close() {
    SD.end();
    sdCardMounted = false;
}

bool SdService::isFile(const std::string filePath) {
    File f = SD.open(filePath.c_str());
    if (f && !f.isDirectory()) {
        return true;
    }  
    return false;     
}

bool SdService::getSdState() {
    return sdCardMounted;
}

std::vector<std::string> SdService::listElements(std::string dirPath, size_t limit) {

    if (limit == 0) {
        limit = globalContext.getFileCountLimit(); 
    };

    std::vector<std::string> filesList;
    std::vector<std::string> foldersList;

    if (!sdCardMounted) {
        return filesList;
    }
    
    File dir = SD.open(dirPath.c_str());
    if (!dir) {
        return filesList;
    }

    if (!dir.isDirectory()) {
        return filesList;
    }

    File file = dir.openNextFile();
    if (!file) {
        return filesList;
    }

    size_t i = 0;
    while (file) {
        // Avoid hidden elements
        if (file.name()[0] != '.') {
            if (file.isDirectory()) {
                foldersList.push_back(file.name());
            } else {
                filesList.push_back(file.name());
            }
            i++;
        }

        // limit
        if (i > limit) {
            break;
        }

        file = dir.openNextFile();
    }

    // Sort both folders and files alphabetically
    std::sort(foldersList.begin(), foldersList.end());
    std::sort(filesList.begin(), filesList.end());

    // Merge both
    foldersList.insert(foldersList.end(), filesList.begin(), filesList.end());
    
    return foldersList;

}

std::string SdService::readFile(const char* filePath) {
    std::string content;
    if (!sdCardMounted) {
        return content;
    }

    File file = SD.open(filePath);
    if (file) {
        while (file.available()) {
            content += (char)file.read();
        }
        file.close();
    }
    return content;
}

bool SdService::writeFile(const char* filePath, const std::string& data) {
    if (!sdCardMounted) {
        return false;
    }

    File file = SD.open(filePath, FILE_WRITE);
    if (file) {
        const uint8_t* buffer = reinterpret_cast<const uint8_t*>(data.c_str());
        file.write(buffer, data.length());
        file.close();
        return true;
    }
    return false;
}

bool SdService::appendToFile(const char* filePath, const std::string& data) {
    if (!sdCardMounted) {
        return false;
    }

    File file = SD.open(filePath, FILE_APPEND);
    if (file) {
        const uint8_t* buffer = reinterpret_cast<const uint8_t*>(data.c_str());
        file.write(buffer, data.length());
        file.close();
        return true;
    }
    return false;
}

}