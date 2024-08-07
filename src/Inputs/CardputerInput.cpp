#include "CardputerInput.h"

namespace inputs {


char CardputerInput::handler() {

    // Update keyboard state
    M5Cardputer.update();

    // Bouton GO
    if (M5Cardputer.BtnA.isPressed()) {
        delay(150); // debounce
        return KEY_RETURN;
    }
    
    if (M5Cardputer.Keyboard.isChange()) {

        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            if (status.enter) { // go to next menu
                return KEY_OK;
            }
            if (status.del) { 
                return KEY_DEL;
            }
            
            if(M5Cardputer.Keyboard.isKeyPressed(KEY_ARROW_LEFT)) { // go back to previous menu
                return KEY_RETURN;
            }

            if(M5Cardputer.Keyboard.isKeyPressed(KEY_ARROW_RIGHT)) { // go to next menu
                return KEY_OK;
            }

            for (auto c : status.word) {
                return c; // retourner le premier char saisi
            }
        }
    }
    delay(10); // debounce
    return KEY_NONE;
}

std::vector<char> CardputerInput::getCommandMappings(const std::vector<RemoteCommand>& commands) {
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