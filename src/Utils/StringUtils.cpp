#include "StringUtils.h"

namespace utils {

std::string StringUtils::cleanCommandName(const std::string& commandName) {
    std::string cleanName;
    for (char c : commandName) {
        if (std::isalnum(c)) {
            cleanName += std::tolower(c);
        }
    }
    return cleanName;
}

std::string StringUtils::removeUnknownPrefix(const std::string& input) {
    const std::string prefix = "Unknown_";
    std::string output;

    if (input.compare(0, prefix.size(), prefix) == 0) {
        // unknown present
        output = input.substr(prefix.size());
    } else {
        output = input;
    }

    return output;
}

std::string StringUtils::removeCsvSuffix(const std::string& input) {
    const std::string extension = ".csv";
    std::string output;

    // Vérifier ext
    if (input.size() >= extension.size() && input.compare(input.size() - extension.size(), extension.size(), extension) == 0) {
        // ext presente
        output = input.substr(0, input.size() - extension.size());
    } else {
        output = input;
    }

    return output;
}

std::vector<std::string> StringUtils::addRemotesPrefixes(const std::vector<std::string>& names) {
    std::vector<std::string> result;
    result.reserve(names.size());

    for (const auto& name : names) {
        result.push_back("Remote " + name);
    }

    return result;
}

bool StringUtils::isUpperString(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](char c) {
        return !std::isalpha(c) || std::isupper(c);
    });
}

std::string StringUtils::toUpperCase(const std::string& input) {
    std::string upperCaseInput = input;
    std::transform(upperCaseInput.begin(), upperCaseInput.end(), upperCaseInput.begin(), ::toupper);
    return upperCaseInput;
}

float StringUtils::getTextCenterOffset(const std::string text, int16_t width, float sizeText) {
    return width / 2 - sizeText * std::strlen(text.c_str()); // to center text
}

std::vector<std::string> StringUtils::convertCharVectorToStringVector(const std::vector<char>& charVector) {
    std::vector<std::string> stringVector;
    stringVector.reserve(charVector.size());

    for (char c : charVector) {
        stringVector.push_back(std::string(1, c));
    }

    return stringVector;
}

std::string StringUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

uint16_t StringUtils::convertToUint16(const std::string& hexString) {
    std::istringstream iss(hexString);
    std::string byteStr;
    uint16_t result = 0;
    int byteCount = 0;

    // Read each byte from the string and shift it into the correct position
    while (iss >> byteStr && byteCount < 2) {  // Limit to the first 2 bytes
        // Convert the byte string to an integer
        uint8_t byte = std::stoul(byteStr, nullptr, 16);
        // Shift the existing result to the left by 8 bits (1 byte) and add the new byte
        result = (result << 8) | byte;
        byteCount++;
    }

    return result;
}

uint16_t* StringUtils::convertToUint16Array(const std::string& dataString, size_t& arraySize) {
    std::istringstream iss(dataString);
    std::string valueStr;
    std::vector<uint16_t> values;

    // Read each value from the string and convert it to uint16_t
    while (iss >> valueStr) {
        uint16_t value = static_cast<uint16_t>(std::stoi(valueStr));
        values.push_back(value);
    }

    // Allocate an array dynamically to hold the values
    arraySize = values.size(); // Store the size of the array
    uint16_t* resultArray = new uint16_t[arraySize];

    // Copy the values from the vector to the array
    for (size_t i = 0; i < arraySize; ++i) {
        resultArray[i] = values[i];
    }

    return resultArray;
}


// Spécialisation pour Product
template <>
std::vector<std::string> StringUtils::extractFieldNames<Product>(const std::vector<Product>& items, const std::string& fieldName) {
    std::vector<std::string> result;
    for (const auto& item : items) {
        if (fieldName == "name") {
            result.push_back(item.name);
        }
    }
    return result;
}

// Spécialisation pour Remote
template <>
std::vector<std::string> StringUtils::extractFieldNames<Remote>(const std::vector<Remote>& items, const std::string& fieldName) {
    std::vector<std::string> result;
    for (const auto& item : items) {
        if (fieldName == "fileName") {
            result.push_back(item.fileName);
        }
    }
    return result;
}

// Spécialisation pour RemoteCommand
template <>
std::vector<std::string> StringUtils::extractFieldNames<RemoteCommand>(const std::vector<RemoteCommand>& items, const std::string& fieldName) {
    std::vector<std::string> result;
    for (const auto& item : items) {
        if (fieldName == "functionName") {
            result.push_back(item.functionName);
        }
    }
    return result;
}

} // namespace utils
