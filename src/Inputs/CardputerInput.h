#ifndef CARDPUTER_KEYBOARD_INPUT_H
#define CARDPUTER_KEYBOARD_INPUT_H

#include <map> 
#include <M5Cardputer.h>
#include <Models/RemoteCommand.h>
#include <Utils/StringUtils.h>

#define KEY_OK '\n'
#define KEY_DEL '\b'
#define KEY_NONE '\0'
#define KEY_RETURN '\r'
#define KEY_ARROW_UP ';'
#define KEY_ARROW_DOWN '.'
#define KEY_ARROW_LEFT ','
#define KEY_ARROW_RIGHT '/'
#define KEY_FAVORITE ' ' // space

using namespace models;

namespace inputs {


class CardputerInput {
public:
    char handler();
    template <typename CommandType>
    std::vector<char> getCommandMappings(const std::vector<CommandType>& commands);

private:

    char assignableKeys[42] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '=', '[',
        ']', '\\', '`'
    };

    // Define some common key mapping
    std::map<std::string, char> keyMappings = {
        {"keypower", 'p'},
        {"power", 'p'},
        {"poweron", 'p'},
        {"keysleep", 'b'},
        {"sleep", 'b'},
        {"hdmi", 'h'},
        {"option", 'i'},
        {"keyplaypause", 'u'},
        {"keyexit", '\\'},
        {"keymode", 'i'},
        {"timer", 't'},
        {"keymenu", '`'},
        {"key1", '1'},
        {"key2", '2'},
        {"key3", '3'},
        {"key4", '4'},
        {"key5", '5'},
        {"key6", '6'},
        {"key7", '7'},
        {"key8", '8'},
        {"key9", '9'},
        {"key0", '0'},
        {"1", '1'},
        {"2", '2'},
        {"3", '3'},
        {"4", '4'},
        {"5", '5'},
        {"6", '6'},
        {"7", '7'},
        {"8", '8'},
        {"9", '9'},
        {"0", '0'},
        {"keyok", 'o'},
        {"keychanneldown", '['},
        {"keychannelup", ']'},
        {"ch-", '['},
        {"ch+", ']'},
        {"heatlo", '['},
        {"heathi", ']'},
        {"chprev", '['},
        {"chnext", ']'},
        {"keyvolumeup", '='},
        {"keyvolumedown", '-'},
        {"coolhi", '='},
        {"coollo", '-'},
        {"volup", '='},
        {"voldn", '-'},
        {"vol+", '='},
        {"vol-", '-'},
        {"volumeup", '='},
        {"volumedown", '-'},
        {"keyup", 'e'},
        {"keydown", 's'},
        {"keyleft", 'a'},
        {"keyright", 'd'},
        {"keymute", 'm'},
        {"mute", 'm'},
        {"keyplay", 'k'},
        {"keystop", 'l'},
        {"keypause", 'j'},
        {"keyforward", 'f'},
        {"keyrewind", 'w'}
    };
};

template <typename CommandType>
std::vector<char> CardputerInput::getCommandMappings(const std::vector<CommandType>& commands) {
    std::vector<char> mappings(commands.size(), '>'); // mapped keys for each command
    std::vector<char> usedKeys; // already assigned keys
    std::vector<std::pair<size_t, char>> defaultKeyBindings; // (index, default key) pairs

    // Collect default keys to avoid conflict
    for (size_t i = 0; i < commands.size(); ++i) {
        std::string cleanName = utils::StringUtils::cleanCommandName(commands[i].functionName);
        auto it = keyMappings.find(cleanName);
        if (it != keyMappings.end()) {
            char mappedKey = it->second;
            if (std::find(usedKeys.begin(), usedKeys.end(), mappedKey) == usedKeys.end()) {
                mappings[i] = mappedKey;
                usedKeys.push_back(mappedKey);
            } else {
                defaultKeyBindings.push_back({i, mappedKey});
            }
        }
    }

    // Assign others keys
    for (size_t i = 0; i < commands.size(); ++i) {
        if (mappings[i] == '>') {
            bool keyFound = false;
            for (char key : assignableKeys) {
                if (std::find(usedKeys.begin(), usedKeys.end(), key) == usedKeys.end()) {
                    mappings[i] = key;
                    usedKeys.push_back(key);
                    keyFound = true;
                    break;
                }
            }
            // stay at default '>' (no keybind)
        }
    }

    // Assign default keys
    for (const auto& binding : defaultKeyBindings) {
        size_t index = binding.first;
        char defaultKey = binding.second;
        if (std::find(usedKeys.begin(), usedKeys.end(), defaultKey) == usedKeys.end()) {
            mappings[index] = defaultKey;
            usedKeys.push_back(defaultKey);
        }
    }

    return mappings;
}

}


#endif // KEYBOARD_H
