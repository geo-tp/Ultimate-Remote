#ifndef COMMAND_SHORTCUTS_H
#define COMMAND_SHORTCUTS_H

#include <algorithm>
#include <Interfaces/IInputProvider.h>
#include <Inputs/InputKeys.h>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <Utils/StringUtils.h>

namespace inputs {

// Uppercase tokens distinguish Fn+letter from ordinary command shortcuts.
// The input provider only emits them while a remote's command list is open.
inline char modifiedCommandKey(char letter) {
    return letter >= 'a' && letter <= 'z' ? static_cast<char>(letter - 'a' + 'A') : '\0';
}

inline std::string commandShortcutLabel(char key) {
    return key >= 'A' && key <= 'Z' ? std::string("Fn+") + key : std::string(1, key);
}

// Lower priorities win independently of the remote's command ordering.
// Previous/Next and PlayPause are useful fallbacks when transport commands
// are absent; every command still gets its own shortcut where keys remain.
inline std::pair<char, int> modifiedCommandShortcut(const std::string& name) {
    std::string clean = utils::StringUtils::cleanCommandName(name);
    if (clean.compare(0, 3, "key") == 0) clean.erase(0, 3);
    static const std::map<std::string, std::pair<char, int>> shortcuts = {
        {"volumedown", {'S', 0}}, {"volume-", {'S', 0}}, {"vol-", {'S', 0}},
        {"voldown", {'S', 0}}, {"voldn", {'S', 0}}, {"volumedn", {'S', 0}},
        {"volumeup", {'D', 0}}, {"volume+", {'D', 0}}, {"vol+", {'D', 0}},
        {"volup", {'D', 0}}, {"mute", {'A', 0}},
        {"play", {'Q', 0}}, {"playpause", {'Q', 1}},
        {"rewind", {'W', 0}}, {"rew", {'W', 0}}, {"rwd", {'W', 0}},
        {"fastrewind", {'W', 0}}, {"fastreverse", {'W', 0}},
        {"previous", {'W', 1}}, {"prev", {'W', 1}}, {"previoussong", {'W', 1}},
        {"previoustrack", {'W', 1}}, {"prevtrack", {'W', 1}}, {"skipback", {'W', 1}},
        {"forward", {'E', 0}}, {"fastforward", {'E', 0}}, {"forw", {'E', 0}},
        {"fwd", {'E', 0}}, {"ffwd", {'E', 0}}, {"ff", {'E', 0}},
        {"next", {'E', 1}}, {"nextsong", {'E', 1}}, {"nexttrack", {'E', 1}},
        {"skipforward", {'E', 1}},
        {"up", {'F', 0}}, {"down", {'X', 0}}, {"right", {'C', 0}}, {"left", {'Z', 0}}
    };
    const auto found = shortcuts.find(clean);
    return found == shortcuts.end() ? std::make_pair('\0', 0) : found->second;
}


inline std::vector<std::string> getCommandLabels(const std::vector<char>& keys) {
    std::vector<std::string> labels;
    labels.reserve(keys.size());
    for (char key : keys) labels.push_back(commandShortcutLabel(key));
    return labels;
}

namespace detail {
    inline constexpr char assignableKeys[42] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '=', '[',
        ']', '\\', '`'
    };

    // Constant mapping registry: no heap allocations at application boot.
    // Deliberately keep lookup exact (the caller normalizes command names).
    inline char mappedKeyForName(const std::string& name) {
        struct Mapping { const char* name; char key; };
        static constexpr Mapping mappings[] = {
{"keypower", 'p'}, {"power", 'p'}, {"poweron", 'p'},
        {"keysleep", 'b'}, {"sleep", 'b'}, {"standby", 'b'}, {"keystandby", 'b'},
        {"hdmi", 'h'}, {"input", 'h'}, {"source", 'h'}, {"keyhdmi", 'h'},
        {"keyinput", 'h'}, {"keysource", 'h'}, {"inputs", 'h'},
        {"option", 'i'}, {"keymode", 'i'}, {"timer", 't'},
        {"keyplaypause", 'u'}, {"playpause", 'u'},
        {"keyexit", '\\'}, {"exit", '\\'}, {"keyreturn", '\\'}, {"return", '\\'},
        {"keyback", '\\'}, {"back", '\\'},
        {"keymenu", '`'}, {"menu", '`'}, {"keyhome", '`'}, {"home", '`'},
        {"key1", '1'}, {"key2", '2'}, {"key3", '3'}, {"key4", '4'}, {"key5", '5'},
        {"key6", '6'}, {"key7", '7'}, {"key8", '8'}, {"key9", '9'}, {"key0", '0'},
        {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'}, {"5", '5'},
        {"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'}, {"0", '0'},
        {"keyok", 'o'}, {"ok", 'o'}, {"keyenter", 'o'}, {"enter", 'o'},
        {"keychanneldown", '['}, {"keychannelup", ']'}, {"ch-", '['}, {"ch+", ']'},
        {"channel-", '['}, {"channel+", ']'}, {"heatlo", '['}, {"heathi", ']'},
        {"chprev", '['}, {"chnext", ']'}, {"fan-", '['}, {"fan+", ']'},
        {"prg-", '['}, {"prg+", ']'},
        {"keyvolumeup", '='}, {"keyvolumedown", '-'}, {"coolhi", '='},
        {"coollo", '-'}, {"heat+", '='}, {"heat-", '-'}, {"volup", '='},
        {"voldn", '-'}, {"vol+", '='}, {"vol-", '-'}, {"volume+", '='},
        {"volume-", '-'}, {"volumeup", '='}, {"volumedown", '-'},
        {"keyup", 'e'}, {"keydown", 's'}, {"keyleft", 'a'}, {"keyright", 'd'},
        {"up", 'e'}, {"down", 's'}, {"left", 'a'}, {"right", 'd'},
        {"keymute", 'm'}, {"mute", 'm'}, {"keyplay", 'k'}, {"keystop", 'l'},
        {"keypause", 'j'}, {"keyforward", 'f'}, {"keyrewind", 'w'},
        {"play", 'k'}, {"stop", 'l'}, {"pause", 'j'}, {"forward", 'f'},
        {"rewind", 'w'}, {"forw", 'f'}, {"rew", 'w'}
        };
        for (const auto& item : mappings) {
            if (name == item.name) return item.key;
        }
        return KEY_NONE;
    }
}

template <typename CommandType>
std::vector<char> getCommandMappings(const std::vector<CommandType>& commands, const interfaces::IInputProvider& input) {
    std::vector<char> mappings(commands.size(), '>');
    std::vector<char> usedKeys;
    std::vector<std::pair<size_t, char>> defaultKeyBindings;
    auto isReservedControlKey = [&input](char key) {
        return key == 'f' || key == 'x' || (input.isZBackEnabled() && key == 'z') ||
               input.isNavigationReservedKey(key);
    };

    if (input.supportsModifiedCommandKeys()) {
        std::map<char, std::pair<size_t, int>> preferred;
        for (size_t i = 0; i < commands.size(); ++i) {
            const auto shortcut = modifiedCommandShortcut(commands[i].functionName);
            if (shortcut.first == KEY_NONE) continue;
            const auto existing = preferred.find(shortcut.first);
            if (existing == preferred.end() || shortcut.second < existing->second.second)
                preferred[shortcut.first] = {i, shortcut.second};
        }
        for (const auto& shortcut : preferred) {
            mappings[shortcut.second.first] = shortcut.first;
            usedKeys.push_back(shortcut.first);
        }
    }

    for (size_t i = 0; i < commands.size(); ++i) {
        if (mappings[i] != '>') continue;
        std::string cleanName = utils::StringUtils::cleanCommandName(commands[i].functionName);
        const char mappedKey = detail::mappedKeyForName(cleanName);
        if (mappedKey != KEY_NONE) {
            if (!isReservedControlKey(mappedKey) &&
                std::find(usedKeys.begin(), usedKeys.end(), mappedKey) == usedKeys.end()) {
                mappings[i] = mappedKey;
                usedKeys.push_back(mappedKey);
            } else {
                defaultKeyBindings.push_back({i, mappedKey});
            }
        }
    }

    for (size_t i = 0; i < commands.size(); ++i) {
        if (mappings[i] == '>') {
            for (char key : detail::assignableKeys) {
                if (!isReservedControlKey(key) &&
                    std::find(usedKeys.begin(), usedKeys.end(), key) == usedKeys.end()) {
                    mappings[i] = key;
                    usedKeys.push_back(key);
                    break;
                }
            }
        }
    }

    for (const auto& binding : defaultKeyBindings) {
        if (!isReservedControlKey(binding.second) &&
            std::find(usedKeys.begin(), usedKeys.end(), binding.second) == usedKeys.end()) {
            mappings[binding.first] = binding.second;
            usedKeys.push_back(binding.second);
        }
    }

    return mappings;
}

}
#endif
