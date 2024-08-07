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

} // namespace utils


// Spécialisation pour Product
template <>
std::vector<std::string> utils::StringUtils::extractFieldNames<Product>(const std::vector<Product>& items, const std::string& fieldName) {
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
std::vector<std::string> utils::StringUtils::extractFieldNames<Remote>(const std::vector<Remote>& items, const std::string& fieldName) {
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
std::vector<std::string> utils::StringUtils::extractFieldNames<RemoteCommand>(const std::vector<RemoteCommand>& items, const std::string& fieldName) {
    std::vector<std::string> result;
    for (const auto& item : items) {
        if (fieldName == "functionName") {
            result.push_back(item.functionName);
        }
    }
    return result;
}